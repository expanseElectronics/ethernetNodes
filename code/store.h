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

#define CONFIG_VERSION "b5g"
#define CONFIG_START 0

enum fx_mode {
  FX_MODE_PIXEL_MAP = 0,
  FX_MODE_12 = 1
};

enum p_type {
  TYPE_DMX_OUT = 0,
  TYPE_RDM_OUT = 1,
  TYPE_DMX_IN = 2,
  TYPE_WS2812 = 3
};

enum p_protocol {
  PROT_ARTNET = 0,
  PROT_ARTNET_SACN = 1
};

enum p_merge {
  MERGE_LTP = 0,
  MERGE_HTP = 1
};

struct StoreStruct {
  char version[4];

  IPAddress ip, subnet, gateway, broadcast, hotspotIp, hotspotSubnet, hotspotBroadcast, dmxInBroadcast;
  bool dhcpEnable, standAloneEnable;
  char nodeName[18], longName[64], wifiSSID[40], wifiPass[40], hotspotSSID[20], hotspotPass[20];
  uint16_t hotspotDelay;
  uint8_t portAmode, portBmode, portAprot, portBprot, portAmerge, portBmerge;
  uint8_t portAnet, portAsub, portAuni[4], portBnet, portBsub, portBuni[4], portAsACNuni[4], portBsACNuni[4];
  uint16_t portAnumPix, portBnumPix, portApixConfig, portBpixConfig;
  bool doFirmwareUpdate;
  uint8_t portApixMode, portBpixMode;
  uint16_t portApixFXstart, portBpixFXstart;
  uint8_t resetCounter, wdtCounter;
  
} deviceSettings = {
  CONFIG_VERSION,
  IPAddress(10, 0, 0, 1), IPAddress(255, 0, 0, 0), IPAddress(10, 0, 0, 1), IPAddress(255, 0, 0, 0), IPAddress(10, 0, 0, 1), IPAddress(255, 0, 0, 0), IPAddress(255, 255, 255, 255), IPAddress(255, 255, 255, 255),
  true, false,
  "expanseNodeG5", "expanseNodeG5", "", "", "expanseNodeG5", "",
  15,
  TYPE_DMX_OUT, TYPE_DMX_OUT, PROT_ARTNET, PROT_ARTNET, MERGE_HTP, MERGE_HTP,
  0, 0, {0, 1, 2, 3}, 0, 0, {4, 5, 6, 7}, {1, 2, 3, 4}, {5, 6, 7, 8},
  680, 680, 0, 0,
  false,
  FX_MODE_PIXEL_MAP, FX_MODE_PIXEL_MAP,
  1, 1,
  0, 0
};


void eepromSave() {
  for (uint16_t t = 0; t < sizeof(deviceSettings); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&deviceSettings + t));
  
  EEPROM.commit();
}

void eepromLoad() {
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]) {

    StoreStruct tmpStore;
    tmpStore = deviceSettings;
    
    for (uint16_t t = 0; t < sizeof(deviceSettings); t++)
      *((char*)&deviceSettings + t) = EEPROM.read(CONFIG_START + t);
    
    if (deviceSettings.resetCounter >= 5 || deviceSettings.wdtCounter >= 10) {
      deviceSettings.wdtCounter = 0;
      deviceSettings.resetCounter = 0;

      deviceSettings = tmpStore;
    }

  } else {
    eepromSave();
    delay(500);
    
    ESP.eraseConfig();
    while(1);
  }
}
