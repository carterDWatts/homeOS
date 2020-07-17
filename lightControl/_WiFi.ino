int connectWiFi(){

  logSB("["+String(millis())+"] Connecting to ["); logSB(ssid); logSB("] with passwd ["); logSB(password); logSBln("]");
  WiFi.begin(ssid.c_str(), password.c_str());
 
  delay(5000);
  logSB("["+String(millis())+"] Wifi stats code: ");
  logSBln( (String) WiFi.status());
  
  if(WiFi.status() == 6){ 
    blinkStatusLed(2, 100, 2);
  }

  if(WiFi.status() == 4){ 
    blinkStatusLed(2, 400, 4);
  }

  if(WiFi.status() == 3){ 
      blinkStatusLed(2, 800, 1);
  }
 
  return WiFi.status();
  
}

String wifiGet(String route){

  HTTPClient http;
  http.begin("http://"+host+":3000"+route);
  String getPayload;
  
  if(http.GET()){
    getPayload = http.getString();
    //logSB("Recieved: ");logSBln(getPayload);
  }else{
    logSBln("Error on HTTP GET request");
    getPayload = "-1";
  }

  http.end();
  return getPayload;
}

String jsonParse(String input, String key){

  deserializeJson(doc, input);
  JsonObject obj = doc.as<JsonObject>();
  return obj[key].as<String>();
}

void wifiPost(String msg){

  //logSBln("["+String(millis())+"] wifiSend started"); 
  
  String postRequest = "light_num=" + lightNum;
    postRequest += "&version=" + String(VERSION);
    postRequest += "&brightness=" + String(BRIGHTNESS);
    //postRequest += " &colors=" + currentP;
    postRequest += "&leds_powered=" + String(ledsPowered);
    postRequest += "&msg=" + msg;

  HTTPClient http;
  http.begin("http://carterwatts.xyz:3000/api");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  if(http.POST(postRequest) > 0){
    String response = http.getString();
    logSB("Response: ");logSBln(response);
  }else{
    logSBln("Error sending POST request");
  }

}
