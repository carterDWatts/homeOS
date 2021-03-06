int connectWiFi(){

  Serial.print("["+String(millis())+"] Connecting to ["); Serial.print(ssid); Serial.print("] with passwd ["); Serial.print(password); Serial.println("]");
  WiFi.begin(ssid.c_str(), password.c_str());
 
  delay(5000);
  Serial.print("["+String(millis())+"] Wifi stats code: ");
  Serial.println(WiFi.status());
  
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
    //Serial.print("Recieved: ");Serial.println(getPayload);
  }else{
    Serial.println("Error on HTTP GET request");
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

  //Serial.println("["+String(millis())+"] wifiSend started"); 
  
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
    Serial.print("Response: ");Serial.println(response);
  }else{
    Serial.println("Error sending POST request");
  }

}
