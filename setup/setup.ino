#include <WiFi.h>
#include "BluetoothSerial.h"
#include <Update.h>
#include <HTTPClient.h>

#include <ArduinoJson.h>

#include <EEPROM.h>

//Meta - TODO: UPDATE FOR EVERY NEW UNIT - ~~~~~~~~~~~~~ NOTICE ~
  String fanNum = "1";
  String location = "Home";
  const String VERSION = "0";
  const int statusLed = 2;
  
  DynamicJsonDocument doc(1024);
  
//WiFi  
  WiFiClient client;
  String ssid = "ORBI24";
  String password = "jaggedcarrot123";  
  
  String host = "carterwatts.xyz"; //TODO:update to new website when up
  
  int port = 3000;//80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
  String bin = "/connectedSoulis.ino.esp32.bin"; // bin file name with a slash in front.

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
    writeEEPROMString(20, fanNum);
    writeEEPROMString(10, PIN);
  
  //UI
    pinMode(statusLed, OUTPUT);
    blinkStatusLed(2, 800, 2);
  
  //Bluetooth
    String bluetoothName = "Soulis #"+fanNum;
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
    wifiPost("Starting", 0,0,0,0,0);   
    
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
    String webVersion = jsonParse(wifiGet("/api/units"), "version");
    Serial.println(wifiGet("/api/units"));
    Serial.println("Current web version: "+webVersion);
    if(webVersion != VERSION && webVersion != "null"){  
      Serial.println("Upgrading to version "+ webVersion+" from " + VERSION);
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
