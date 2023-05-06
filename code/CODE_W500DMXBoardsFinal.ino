/*
dualETH Ethernet ArtNet Node 

Base Code Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/
Ethernet Implementation Copyright (c) 2023, expanseElectronics Ltd
https://github.com/expanseElectronics/

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/

#include <SPI.h>
#include <EthernetLarge.h>
#include <EthernetWebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <FS.h>

#include "store.h"
#include "ethEspDMX_RDM.h"
#include "ethEspArtNetRDM.h"
#include "ethWs2812Driver.h"
#include "index.h"
#include "css.h"

extern "C" {
#include "user_interface.h"
  extern struct rst_info resetInfo;
}

#define FIRMWARE_VERSION "v5.72"
#define ART_FIRM_VERSION 0x0572
#define ARTNET_OEM 0x2BBA
#define ESTA_MAN 0x7D00
#define ESTA_DEV 0xEE000000

#define DMX_DIR_A 16
#define DMX_TX_A 1
#define DMX_TX_B 2

#define STATUS_LED_PIN 4
#define STATUS_LED_MODE_WS2812
#define STATUS_LED_I 3
#define STATUS_LED_A 2
#define STATUS_LED_B 1
#define STATUS_LED_S 0

#define WS2812_ALLOW_INT_SINGLE false
#define WS2812_ALLOW_INT_DOUBLE false

#define BLACK 0x00000000
#define WHITE 0x00FFFFFF
#define RED 0x0000FF00
#define GREEN 0x000000FF
#define BLUE 0x00FF0000
#define CYAN 0x00FF00FF
#define PINK 0x0066FF22
#define MAGENTA 0x00FFFF00
#define YELLOW 0x0000FFFF
#define ORANGE 0x0000FF33
#define STATUS_DIM 0x04

byte mac[6] = {
  0xBC, 0xFF, 0x4D, 0x45, 0x61, 0x0D
};

uint8_t portA[5], portB[5];
uint8_t dmxInSeqID = 0;
uint8_t statusLedData[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t statusTimer = 0;

esp8266ArtNetRDM artRDM;
EthernetWebServer webServer(80);
DynamicJsonBuffer jsonBuffer;
ws2812Driver pixDriver;
File fsUploadFile;
bool statusLedsDim = true;
bool statusLedsOff = false;

const char PROGMEM typeHTML[] = "text/html";
const char PROGMEM typeCSS[] = "text/css";

char wifiStatus[60] = "";
bool isHotspot = false;
uint32_t nextNodeReport = 0;
char nodeError[ARTNET_NODE_REPORT_LENGTH] = "";
bool nodeErrorShowing = 1;
uint32_t nodeErrorTimeout = 0;
bool pixDone = true;
bool newDmxIn = false;
bool doReboot = false;
byte* dataIn;


void setup(void) {
  pinMode(DMX_DIR_A, OUTPUT);
  digitalWrite(DMX_DIR_A, LOW);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(10);

  Ethernet.init(15);

  setStatusLed(STATUS_LED_S, CYAN);
  doStatusLedOutput();

  wifi_set_sleep_type(NONE_SLEEP_T);
  bool resetDefaults = false;

  EEPROM.begin(1024);

  SPIFFS.begin();

  if (SPIFFS.exists("/formatted.txt")) {
    SPIFFS.format();

    File f = SPIFFS.open("/formatted.txt", "w");
    f.print("Formatted");
    f.close();
  }

  if (!resetDefaults) eepromLoad();

  if (resetInfo.reason != REASON_DEFAULT_RST && resetInfo.reason != REASON_EXT_SYS_RST && resetInfo.reason != REASON_SOFT_RESTART)
    deviceSettings.wdtCounter++;
  else
    deviceSettings.resetCounter++;

  eepromSave();

  wifiStart();

  webStart();

  if (!deviceSettings.doFirmwareUpdate && deviceSettings.wdtCounter <= 3) {
    if (deviceSettings.portAmode == TYPE_DMX_IN && deviceSettings.portBmode == TYPE_RDM_OUT)
      deviceSettings.portBmode = TYPE_DMX_OUT;
    artStart();

    while (millis() < 3500)
      yield();

    portSetup();

  } else
    deviceSettings.doFirmwareUpdate = false;

  delay(10);
}

void loop(void) {
  if (deviceSettings.resetCounter != 0 && millis() > 6000) {
    deviceSettings.resetCounter = 0;
    deviceSettings.wdtCounter = 0;
    eepromSave();
  }

  webServer.handleClient();
  doNodeReport();
  artRDM.handler();

  yield();

  dmxA.handler();
  dmxB.handler();

  if (newDmxIn) {
    uint8_t g, p, n;

    newDmxIn = false;

    g = portA[0];
    p = portA[1];

    IPAddress bc = deviceSettings.dmxInBroadcast;
    artRDM.sendDMX(g, p, bc, dataIn, 512);

    setStatusLed(STATUS_LED_I, CYAN);
  }

  if (doReboot) {
    char c[ARTNET_NODE_REPORT_LENGTH] = "Device rebooting...";
    artRDM.setNodeReport(c, ARTNET_RC_POWER_OK);
    artRDM.artPollReply();

    uint32_t n = millis() + 1000;
    while (millis() < n)
      webServer.handleClient();

    ESP.restart();
  }

  if (statusTimer < millis()) {
    if ((statusTimer % 2000) > 1000) {
      setStatusLed(STATUS_LED_S, BLACK);
    }

    else if (nodeError[0] != '\0') {
      setStatusLed(STATUS_LED_S, RED);
      setStatusLed(STATUS_LED_A, BLACK);
      setStatusLed(STATUS_LED_B, BLACK);
      setStatusLed(STATUS_LED_I, BLACK);
    }

    else {
      setStatusLed(STATUS_LED_S, GREEN);
    }

    doStatusLedOutput();
    statusTimer = millis() + 1000;
  }
}

void dmxHandle(uint8_t group, uint8_t port, uint16_t numChans, bool syncEnabled) {
  if (portA[0] == group) {
    if (deviceSettings.portAmode != TYPE_DMX_IN && port == portA[1]) {
      dmxA.chanUpdate(numChans);
      setStatusLed(STATUS_LED_A, BLUE);
    }
  }
  else if (portB[0] == group) {
    if (deviceSettings.portBmode != TYPE_DMX_IN && port == portB[1]) {
      dmxB.chanUpdate(numChans);
      setStatusLed(STATUS_LED_B, BLUE);
    }
  }
}

void syncHandle() {
  if (deviceSettings.portAmode != TYPE_DMX_IN)
    dmxA.unPause();
  if (deviceSettings.portBmode != TYPE_DMX_IN)
    dmxB.unPause();
}

void ipHandle() {
  if (artRDM.getDHCP()) {
    deviceSettings.gateway = INADDR_NONE;
    deviceSettings.dhcpEnable = 1;
    doReboot = true;
  }

  else {
    deviceSettings.ip = artRDM.getIP();
    deviceSettings.subnet = artRDM.getSubnetMask();
    deviceSettings.gateway = deviceSettings.ip;
    deviceSettings.gateway[3] = 1;
    deviceSettings.broadcast = {~deviceSettings.subnet[0] | (deviceSettings.ip[0] & deviceSettings.subnet[0]), ~deviceSettings.subnet[1] | (deviceSettings.ip[1] & deviceSettings.subnet[1]), ~deviceSettings.subnet[2] | (deviceSettings.ip[2] & deviceSettings.subnet[2]), ~deviceSettings.subnet[3] | (deviceSettings.ip[3] & deviceSettings.subnet[3])};
    deviceSettings.dhcpEnable = 0;
    doReboot = true;
  }
  eepromSave();
}

void addressHandle() {
  memcpy(&deviceSettings.nodeName, artRDM.getShortName(), ARTNET_SHORT_NAME_LENGTH);
  memcpy(&deviceSettings.longName, artRDM.getLongName(), ARTNET_LONG_NAME_LENGTH);

  deviceSettings.portAnet = artRDM.getNet(portA[0]);
  deviceSettings.portAsub = artRDM.getSubNet(portA[0]);
  deviceSettings.portAuni[0] = artRDM.getUni(portA[0], portA[1]);
  deviceSettings.portAmerge = artRDM.getMerge(portA[0], portA[1]);

  if (artRDM.getE131(portA[0], portA[1]))
    deviceSettings.portAprot = PROT_ARTNET_SACN;
  else
    deviceSettings.portAprot = PROT_ARTNET;

  deviceSettings.portBnet = artRDM.getNet(portB[0]);
  deviceSettings.portBsub = artRDM.getSubNet(portB[0]);
  deviceSettings.portBuni[0] = artRDM.getUni(portB[0], portB[1]);
  deviceSettings.portBmerge = artRDM.getMerge(portB[0], portB[1]);

  if (artRDM.getE131(portB[0], portB[1]))
    deviceSettings.portBprot = PROT_ARTNET_SACN;
  else
    deviceSettings.portBprot = PROT_ARTNET;

  eepromSave();
}

void rdmHandle(uint8_t group, uint8_t port, rdm_data* c) {
  if (portA[0] == group && portA[1] == port)
    dmxA.rdmSendCommand(c);

  else if (portB[0] == group && portB[1] == port)
    dmxB.rdmSendCommand(c);
}

void rdmReceivedA(rdm_data* c) {
  artRDM.rdmResponse(c, portA[0], portA[1]);
}

void sendTodA() {
  artRDM.artTODData(portA[0], portA[1], dmxA.todMan(), dmxA.todDev(), dmxA.todCount(), dmxA.todStatus());
}

void rdmReceivedB(rdm_data* c) {
  artRDM.rdmResponse(c, portB[0], portB[1]);
}

void sendTodB() {
  artRDM.artTODData(portB[0], portB[1], dmxB.todMan(), dmxB.todDev(), dmxB.todCount(), dmxB.todStatus());
}

void todRequest(uint8_t group, uint8_t port) {
  if (portA[0] == group && portA[1] == port)
    sendTodA();

  else if (portB[0] == group && portB[1] == port)
    sendTodB();
}

void todFlush(uint8_t group, uint8_t port) {
  if (portA[0] == group && portA[1] == port)
    dmxA.rdmDiscovery();

  else if (portB[0] == group && portB[1] == port)
    dmxB.rdmDiscovery();
}

void dmxIn(uint16_t num) {
  byte* tmp = dataIn;
  dataIn = dmxA.getChans();
  dmxA.setBuffer(tmp);

  newDmxIn = true;
}

void doStatusLedOutput() {
  uint8_t a[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  if (!statusLedsOff) {
    if (statusLedsDim) {
      for (uint8_t x = 0; x < 12; x++)
        a[x] = statusLedData[x] & STATUS_DIM;
    } else {
      for (uint8_t x = 0; x < 12; x++)
        a[x] = statusLedData[x];
    }
  }

  pixDriver.doPixel(&a[0], STATUS_LED_PIN, 12);

  for (uint8_t x = 1; x < 12; x += 3)
    statusLedData[x] = 125;
}

void setStatusLed(uint8_t num, uint32_t col) {
  memcpy(&statusLedData[num * 3], &col, 3);
}
