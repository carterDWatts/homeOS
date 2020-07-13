#include <WiFi.h>
#include "BluetoothSerial.h"
#include <Update.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

#include <EEPROM.h>

//TODO: make method that removes spaces from msg and places %20 in place, set blink status led to switch case and document cases,
//      add timer to count time leds shut off from heat

//Meta - TODO: UPDATE FOR EVERY NEW UNIT - ~~~~~~~~~~~~~ NOTICE ~
  String fanNum = "";
  String location = "";
  const String VERSION = "0.7.3.2";
  const int statusLed = 2;

  DynamicJsonDocument doc(1024);

//LEDS (UVC)
  boolean ledsPowered = true;
  boolean ledsPoweredLast = false;
  const int ledSwitch = 13;
  
  const double MS_IN_HOURS = 3600000;
  double hoursRunning = 0;
  double timePassed  = 0;
  double lastTimeRunning;
       
//WiFi  
  WiFiClient client;
  String ssid = "";
  String password = "";  
  
  String host = "carterwatts.xyz"; //TODO:update to new website when up
  
  int port = 3000;//80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
  String bin = "/connectedSoulis.ino.esp32.bin"; // bin file name with a slash in front.
  
//Bluetooth
  BluetoothSerial bluetooth; 
  String bluetoothBuffer = "";
  String PIN = "";
  String masterPIN = "11701";
  
//Sensor
  //Current
    const int currentSens1 = A0;
    const int mVperAmp = 100; //100 for 20A Module and 66 for 30A Module  

  //Temp & Hum
    //One dht is used here for internal and one for external
    #define DHTTYPE DHT11
    #define DHTPINA 4
    #define DHTPINB 16
    DHT dhtA(DHTPINA,DHTTYPE);
    DHT dhtB(DHTPINB,DHTTYPE);
  //couple 
    //uses software spi

    double highTemp = 33;
    //All thermocoupples share these pins
    #define MAXCS   23
    #define MAXCLK  22
      
    //Each thermocpule has one of these pins for data
    #define MAXDOa   27
    Adafruit_MAX31855 thermocoupleA(MAXCLK, MAXCS, MAXDOa);
    #define MAXDOb   26
    Adafruit_MAX31855 thermocoupleB(MAXCLK, MAXCS, MAXDOb);
    #define MAXDOc   25
    Adafruit_MAX31855 thermocoupleC(MAXCLK, MAXCS, MAXDOc);
      
void setup(){
  
  //Serial
    Serial.begin(115200);
    Serial.println();
    Serial.print("["+String(millis())+"] Setup started Version ");
    Serial.println(VERSION);

  //EEPROM
    EEPROM.begin(512);  

  //META
    fanNum = readEEPROMString(20);
    location = readEEPROMString(300);
    PIN = readEEPROMString(10);
    
  //UI
    pinMode(statusLed, OUTPUT);
    blinkStatusLed(2, 800, 2);

  //Bluetooth
    String bluetoothName = "Soulis #"+fanNum;
    bluetooth.begin(bluetoothName);
    
  //WiFi
    ssid = readEEPROMString(200);
    password = readEEPROMString(100);
  
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.enableSTA(true);
    connectWiFi();
         
  //Sensors
    dhtA.begin();
    dhtB.begin();
    
  //Controll
    pinMode(ledSwitch, OUTPUT);
    
  //UI
    blinkStatusLed(2, 200, 3);
    wifiPost("Starting", 0,0,0,0,0);
    Serial.println("["+String(millis())+"] Setup complete");
}

void loop(){

  String msg = "";

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

  //TEST
    Serial.println(wifiGet("/api/"+fanNum));
      
  //OTA 
    String webVersion = jsonParse(wifiGet("/api/units"), "version");
    if(webVersion != VERSION && webVersion != "null"){  
      Serial.println("Upgrading to version "+ webVersion+" from " + VERSION);
      wifiPost("Updating",0,0,0,0,0); 
      execOTA();
    }
    if(webVersion == "null"){
      Serial.println("ERROR webVersion returned null");
    }

  //Temp & Hum
    //DHTs get values
      double dhtTempA = dhtA.readTemperature();
      double dhtTempB = dhtB.readTemperature();
      
    //Thermocouples
       //A get value of thermocouple & check if working
         double thermoTempA = thermocoupleA.readCelsius();
         if (isnan(thermoTempA)) {
           Serial.println("[ERROR] could not read thermocouple A");
           msg += " err_reading_thermocouple_A, ";
         }
       //B get value of thermocouple & check if working
        double thermoTempB = thermocoupleB.readCelsius();
        if (isnan(thermoTempB)) {
          Serial.println("[ERROR] could not read thermocouple B");
          msg += " err_reading_thermocouple_B, ";
        }
       //C get value of thermocouple & check if working
        double thermoTempC = thermocoupleC.readCelsius();
        if (isnan(thermoTempC)) {
          Serial.println("[ERROR] could not read thermocouple C");
          msg += " err_reading_thermocouple_C ";
        }

    //Check if thermocouples are detecting overheating
    if(thermoTempA > highTemp || thermoTempB > highTemp || thermoTempC > highTemp){
      ledsPowered = false;              //Turns lights off
      msg += " lights_overheated ";     //Tells web lights are over heat
    }else if(jsonParse(wifiGet("/api/units"), "webState") != "on"){
      ledsPowered = false;              //Force lights off by web
      msg += "leds_shut_down_by_web ";  //Tells web lights are turned off by web
    }else{
      ledsPowered = true;
    }

    if(msg == "") msg = "running";
    
  //WiFi Post
    wifiPost(msg, thermoTempA, thermoTempB, thermoTempC, dhtTempA, dhtTempB); 
    if(WiFi.status() != 3){ 
      connectWiFi();
      return;
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
