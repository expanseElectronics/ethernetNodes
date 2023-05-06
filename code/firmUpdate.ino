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

void webFirmwareUpdate() {
  String fail = "{\"success\":0,\"message\":\"Unknown Error\"}";
  String ok = "{\"success\":1,\"message\":\"Success: Device restarting\"}";

  webServer.sendHeader("Connection", "close");
  webServer.sendHeader("Access-Control-Allow-Origin", "*");
  webServer.send(200, "application/json", (Update.hasError()) ? fail : ok);

  doReboot = true;
}

void webFirmwareUpload() {
  String reply = "";
  ethernetHTTPUpload& upload = webServer.upload();
    
  if(upload.status == UPLOAD_FILE_START){
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if(!Update.begin(maxSketchSpace)){//start with max available size
      reply = "{\"success\":0,\"message\":\"Insufficient space.\"}";
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      reply = "{\"success\":0,\"message\":\"Failed to save\"}";
    }
    
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ 
      reply = "{\"success\":1,\"message\":\"Success: Device Restarting\"}";
    } else {
      reply = "{\"success\":0,\"message\":\"Unknown Error\"}";
    }
  }
  yield();
  
  if (reply.length() > 0) {
    webServer.sendHeader("Connection", "close");
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", reply);
  }
}
