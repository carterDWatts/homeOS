void FillLEDsFromPaletteColors( uint8_t colorIndex){

  logSBln("Running FillLEDsFromPaletteColors");

  uint8_t brightness = 255;    
  for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      colorIndex += 3;
  }
}

void pulseLEDs(uint8_t colorIndex){

  logSBln("Running fade");
  
  uint8_t brightness = 0;
  int MAX = 150, MIN = 0;
  for( int i = 0; i < NUM_LEDS; i++) {
    switch(fadeColor){
        case 5:
          leds[i] = CHSV( HUE_RED, 255, 255);
          break;
        case 6:
          leds[i] = CHSV( HUE_PINK, 255, 255);
          break;
        case 7:
          leds[i] = CRGB( 105, 255, 255); //Baby Blue
          break;
        case 8:
          leds[i] = CHSV( HUE_YELLOW, 255, 255);
          break;
        case 9:
          leds[i] = CHSV( HUE_GREEN, 255, 255);
          break;
    }
  }

  for(int i = MIN; i < MAX; i++){
    FastLED.setBrightness(i); 
    FastLED.show();
    delay(10);
  }
  for(int i = MAX; i > MIN; i--){
    FastLED.setBrightness(i);
    FastLED.show();
    delay(10);
  }
  
}

void SetRandomPalette(){
  
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void SetPinkAndBabyBluePalette(){

    logSBln("setting to Pink and Baby Blue");
    
    CRGB pink  = CHSV( HUE_PINK, 255, 255);
    CRGB blue  = CHSV( HUE_BLUE, 255, 255);
    CRGB babyBlue = CRGB( 105, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   babyBlue,  babyBlue,  babyBlue,  black,
                                   pink, pink, pink,  black,
                                   babyBlue,  babyBlue,  babyBlue,  black,
                                   pink, pink, pink,  black );
}

void SetRedAndBlackPalette(){
  
    logSBln("setting to Red and Black");
  
    CRGB red  = CHSV( HUE_RED, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   red,  red,  red,  black,
                                   red, red, red,  black,
                                   red,  red,  red,  black,
                                   red, red, red,  black );
}

void SetGreenAndYellowPalette(){

    logSBln("setting to Green and Yellow");
  
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB yellow = CHSV( HUE_YELLOW, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  green,  black,
                                   yellow, yellow, yellow,  black,
                                   green,  green,  green,  black,
                                   yellow, yellow, yellow,  black );
}
