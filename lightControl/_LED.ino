void FillLEDsFromPaletteColors( uint8_t colorIndex){
  
    uint8_t brightness = 255;    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void pulseLEDs(uint8_t colorIndex){

  uint8_t brightness = 0;
  for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV( HUE_RED, 255, 255); //ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
  }

  for(int i = 0; i < 255; i++){
    FastLED.setBrightness(i); 
    FastLED.show();
  }
  for(int i = 255; i > 0; i--){
    FastLED.setBrightness(i);
    FastLED.show();
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
