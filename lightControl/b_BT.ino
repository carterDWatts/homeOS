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
  
  logSBln(sectionA);
  logSBln(sectionB);
  logSBln(sectionC);
  
  logSB("BT recieved: "); logSBln(command);
  
  if(sectionB == "ssid"){
    ssid = sectionC;
    writeEEPROMString(200, ssid);
    logSB("ssid set to ");logSBln(readEEPROMString(200));
    bluetooth.print("ssid set to ");bluetooth.println(readEEPROMString(200));
    connectWiFi();
    return;
  }
  if(sectionB == "passwd"){
    password = sectionC;
    writeEEPROMString(100, password);
    logSB("password set to ");logSBln(readEEPROMString(100));
    bluetooth.print("password set to ");bluetooth.println(readEEPROMString(100));
    connectWiFi();
    return;
  }
  if(sectionB == "PIN"){
    if(sectionC.length() == 5){
      PIN = sectionC;
      writeEEPROMString(10, PIN);
      logSB("PIN set to ");logSBln(readEEPROMString(10));
      bluetooth.print("PIN set to "); bluetooth.println(readEEPROMString(10));
      return;
    }else{
      return;
    }
  }

  if(sectionB == "color"){
    
    String btPal = sectionC;
    logSBln("Changing palette");

    if(btPal.equals("1")){
      SetRedAndBlackPalette();
      logSBln("BT set color to 1");
      wifiPostColor("Setting to color scheme 1", btPal);
    }
    if(btPal.equals("2")){
      SetPinkAndBabyBluePalette();
      logSBln("BT set color to 2");
      wifiPostColor("Setting to color scheme 2", btPal);
    }
    if(btPal.equals("3")){
      SetGreenAndYellowPalette();
      logSBln("BT set color to 3");
      wifiPostColor("Setting to color scheme 3", btPal);
    }
    if(btPal.equals("4")){
      SetRandomPalette();
      logSBln("BT set color to 4");
      wifiPostColor("Setting to color scheme 4", btPal);
    }
  }
                 
          
  /*
  if(sectionB == "location"){
    if(sectionC.length() < 50){
      location = sectionC;
      writeEEPROMString(300, location);
      logSB("location set to ");logSBln(readEEPROMString(300));
      bluetooth.print("location set to "); bluetooth.println(readEEPROMString(300));
      return;
    }else{
      bluetooth.println("Location cannot be longer than 50 characters");
      return;
    }
  }
  */
  
  bluetooth.println("Invalid command");
  logSBln("Invalid command");
  return;
  
}
