#include <WiFi.h>
#include "BluetoothSerial.h"
#include <Update.h>
#include <HTTPClient.h>

#include <ArduinoJson.h>

#include <EEPROM.h>

//Meta - TODO: UPDATE FOR EVERY NEW UNIT - ~~~~~~~~~~~~~ NOTICE ~
  String lightNum = "2";
  const String VERSION = "0";
  const int statusLed = 2;
  
  DynamicJsonDocument doc(1024);
  
//WiFi  
  WiFiClient client;
  String ssid = "ORBI24";
  String password = "jaggedcarrot123";  
  
  String host = "carterwatts.xyz"; //TODO:update to new website when up  
  int port = 3000;//80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
  String bin = "/lightControl.ino.esp32.bin"; // bin file name with a slash in front.

//Bluetooth
  BluetoothSerial bluetooth; 
  String bluetoothBuffer = "";
  String PIN = "12345";
  String masterPIN = "11701";

void setup(){

  //Serial
    Serial.begin(115200);
    Serial.println();
    Serial.print("["+String(millis())+"] Setup started initial version ");
    Serial.println(VERSION);
  
  //EEPROM
    EEPROM.begin(512);

  //META
    writeEEPROMString(20, lightNum);
    writeEEPROMString(10, PIN);
  
  //UI
    pinMode(statusLed, OUTPUT);
    blinkStatusLed(2, 800, 2);
  
  //Bluetooth
    String bluetoothName = "Light #"+lightNum;
    bluetooth.begin(bluetoothName);
      
  //WiFi
    writeEEPROMString(200, ssid);
    writeEEPROMString(100, password); 
    
    ssid = readEEPROMString(200);
    password = readEEPROMString(100);
    
    
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.enableSTA(true);    
    connectWiFi();
         
  //UI
    blinkStatusLed(2, 200, 3);
    wifiPost("Starting setup software");   
    
  Serial.println("["+String(millis())+"] Setup complete");
}

void loop() {

  //Bluetooth
    if (bluetooth.available()){
      char incoming = bluetooth.read();
      bluetoothBuffer += incoming;
      if(bluetoothBuffer.length() < 100){
        return;
      }
    }
    if(bluetoothBuffer != ""){
      bluetoothTree(bluetoothBuffer);
    }
    bluetoothBuffer = "";

  //WiFi
    if(WiFi.status() != 3){ 
      connectWiFi();
      return;
    }
    
  //OTA 
    String webVersion = jsonParse(wifiGet("/api/lights"), "version");
    Serial.println(wifiGet("/api/lights"));
    Serial.println("Current web version: "+webVersion);
    if(webVersion != VERSION && webVersion != "null"){  
      Serial.println("Upgrading to version "+ webVersion+" from " + VERSION);
      wifiPost("Updating to current version");
      execOTA();
    }   

    
}

void blinkStatusLed(int pin, long delayTime, int iter){
  
  for(int i = 0; i < iter; i++){
    digitalWrite(pin, HIGH);
    delay(delayTime);
    digitalWrite(pin, LOW);
    delay(delayTime);
  }
}
