#include <WiFi.h>
#include "BluetoothSerial.h"
#include <Update.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <FastLED.h>

//Meta - TODO: UPDATE FOR EVERY NEW UNIT - ~~~~~~~~~~~~~ NOTICE ~
  String lightNum = "";
  const String VERSION = "0.5.2.0";
  const int ledDataPin = 5;
  const int statusLed = 2;

  DynamicJsonDocument doc(1024);

//LEDS
  int ledsPowered = 1;  
  #define LED_PIN     27
  #define NUM_LEDS    180 //180
  int BRIGHTNESS = 0;
  #define LED_TYPE    WS2811
  #define COLOR_ORDER GRB
  CRGB leds[NUM_LEDS];

  #define UPDATES_PER_SECOND 30
  
  CRGBPalette16 currentPalette;
  TBlendType    currentBlending;
  
  extern CRGBPalette16 myRedWhiteBluePalette;
  extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
  unsigned long prevMillis;
  String currentP;
  int fadeColor;
  
//WiFi  
  WiFiClient client;
  String ssid = "";
  String password = "";  
  
  String host = "carterwatts.xyz"; //TODO:update to new website when up  
  int port = 3000;//80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.  
  String bin = "/lightControl.ino.esp32.bin"; // bin file name with a slash in front.
  boolean runningLEDs = true;
  boolean UPDATING = false;
  
//Bluetooth
  BluetoothSerial bluetooth; 
  String bluetoothBuffer = "";
  String PIN = "";
  String masterPIN = "11701";

//Core management
  TaskHandle_t manageH = NULL;
  TaskHandle_t displayLEDsH = NULL;
      
void setup(){
  
   //Serial
    Serial.begin(115200);
    logSBln(" ");
    logSB("["+String(millis())+"] Setup started Version ");
    logSBln(VERSION);
    
  //EEPROM
    EEPROM.begin(512); 
    
  //META
    lightNum = readEEPROMString(20);
    PIN = readEEPROMString(10);
    
  //UI
    pinMode(statusLed, OUTPUT);
    blinkStatusLed(2, 800, 2);

  //Bluetooth
    String bluetoothName = "Light #"+lightNum;
    bluetooth.begin(bluetoothName);

  //LEDs
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB::Black;
    }
    FastLED.show();
        
    currentBlending = LINEARBLEND;

    SetPinkAndBabyBluePalette();
    currentP = "2";
    fadeColor = 5; //Default fade color is red
        
  //WiFi
    ssid = readEEPROMString(200);
    password = readEEPROMString(100);
  
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.enableSTA(true);
    connectWiFi();

  //UI
    blinkStatusLed(2, 200, 3);
    wifiPost("Starting");
    logSBln("["+String(millis())+"] Setup complete");
    
  //Core management
    xTaskCreatePinnedToCore(
      manage, /* Function to implement the task */
      "manage", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &manageH, //&manage,  /* Task handle. */
      0); /* Core where the task should run */

    xTaskCreatePinnedToCore(
      displayLEDs, /* Function to implement the task */
      "displayLEDs", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &displayLEDsH,  /* Task handle. */
      1); /* Core where the task should run */
 
}

void displayLEDs(void * params){

  while(1){
      
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    int palInt = currentP.toInt();
    logSB("palInt: ");logSBln((String)palInt);
    
    if(palInt >= 5 && palInt <= 9){
      logSBln("Fade");
      pulseLEDs(startIndex);
    }
    
    if(palInt <= 4 && palInt > 0){
      logSBln("Snake");
      BRIGHTNESS = 200;
      FastLED.setBrightness(BRIGHTNESS);
      FillLEDsFromPaletteColors(startIndex);
      FastLED.show();
      FastLED.delay(1000 / UPDATES_PER_SECOND);
    }
    
    if(UPDATING){
      vTaskDelete(NULL);
    }
  }
}


void manage(void * params){

  while(1){
    while(1){
      
      //Bluetooth
        if (bluetooth.available()){
          char incoming = bluetooth.read();
          bluetoothBuffer += incoming;
          if(bluetoothBuffer.length() < 100){
            break;
          }
        }
        if(bluetoothBuffer != ""){
          bluetoothTree(bluetoothBuffer);
        }
        bluetoothBuffer = "";
      
      //Check/setup wifi before use
        if(WiFi.status() != 3){ 
          connectWiFi();
          break;
        }
        String msg = "";
      
      //TEST
        String cleanedArr = wifiGet("/api/"+lightNum);
        if(cleanedArr != NULL){
          cleanedArr.remove(0,1);
          cleanedArr.remove(cleanedArr.length()-1, 1);
          
          String webP = jsonParse(cleanedArr, "colors");
          logSBln(webP);
          if(webP != NULL && currentP != webP){
            currentP = webP;
            logSBln("changing palette");
            if(currentP.equals("1")) SetRedAndBlackPalette();
            if(currentP.equals("2")) SetPinkAndBabyBluePalette();
            if(currentP.equals("3")) SetGreenAndYellowPalette();
            if(currentP.equals("4")) SetRandomPalette();            
            if(currentP.equals("5")) fadeColor = 5; //Red         
            if(currentP.equals("6")) fadeColor = 6; //Pink    
            if(currentP.equals("7")) fadeColor = 7; //Blue                
            if(currentP.equals("8")) fadeColor = 8; //Yellow  
            if(currentP.equals("9")) fadeColor = 9; //Green        
          }
          
          int webB = jsonParse(cleanedArr, "brightness").toInt();
          if(webP != NULL && webB != BRIGHTNESS){
            //logSBln("Changing brightness");
            //BRIGHTNESS = webB;
            //FastLED.setBrightness(BRIGHTNESS);
          }
        }
        
      //OTA
        String webVersion = jsonParse(wifiGet("/api/lights"), "version");
        if(webVersion != NULL && webVersion != "null" && webVersion != VERSION ){  
          logSBln("Upgrading to version "+ webVersion+" from " + VERSION);
          wifiPost("Updating");
          UPDATING = true;
          disableCore0WDT();
          vTaskDelay(20 / portTICK_PERIOD_MS);
          execOTA();
        }
        if(webVersion == "null"){
          logSBln("ERROR webVersion returned null");
        }
    
        if(msg == "") msg = "running";
        
      //WiFi Post
        wifiPost(msg);       
    }
  }
}

void loop(){vTaskDelete(NULL);}

void blinkStatusLed(int pin, long delayTime, int iter){
  
  for(int i = 0; i < iter; i++){
    digitalWrite(pin, HIGH);
    delay(delayTime);
    digitalWrite(pin, LOW);
    delay(delayTime);
  }
}

void logSB(String line){
  Serial.print(line);
  bluetooth.print(line);
}

void logSBln(String line){
  Serial.println(line);
  bluetooth.println(line);
}
