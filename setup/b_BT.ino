void bluetoothTree(String command){

  if(command == "help"){
    String helpMenu = "Help\nCommands:\nssid\n+passwd\n+PIN\n+location\nrun [command] -h for help with a specific command";
    bluetooth.print(helpMenu);
    return;
  }

  int index1 = command.indexOf(" ");
  int index2 = command.indexOf(" ",index1+1);
  
  String sectionA = command.substring(0, index1);  
  String sectionB = command.substring(index1+1, index2);  
  String sectionC = command.substring(index2+1);
  
  Serial.println(sectionA);
  Serial.println(sectionB);
  Serial.println(sectionC);
  
  Serial.print("BT recieved: "); Serial.println(command);
  
  if(sectionB == "ssid"){
    ssid = sectionC;
    writeEEPROMString(200, ssid);
    Serial.print("ssid set to ");Serial.println(readEEPROMString(200));
    bluetooth.print("ssid set to ");bluetooth.println(readEEPROMString(200));
    connectWiFi();
    return;
  }
  if(sectionB == "passwd"){
    password = sectionC;
    writeEEPROMString(100, password);
    Serial.print("password set to ");Serial.println(readEEPROMString(100));
    bluetooth.print("password set to ");bluetooth.println(readEEPROMString(100));
    connectWiFi();
    return;
  }
  if(sectionB == "PIN"){
    if(sectionC.length() == 5){
      PIN = sectionC;
      writeEEPROMString(10, PIN);
      Serial.print("PIN set to ");Serial.println(readEEPROMString(10));
      bluetooth.print("PIN set to "); bluetooth.println(readEEPROMString(10));
      return;
    }else{
      bluetooth.println("Pin must be 5 chars");
      return;
    }
  }
  
  bluetooth.println("Invalid command");
  Serial.println("Invalid command");
  return;
  
}
