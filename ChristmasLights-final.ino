#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

/*******************
 * Global constants
 *******************/
#define DATA_PIN 16
#define LED_TYPE WS2811
#define COLOR_ORDER RGB

const int buttonUpPin = 4;
const int buttonDnPin = 5;
const int MAX_NUM = 27;
const int NUM_LEDS = 100;
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2c_addr = 0x27;
const int BRIGHTNESS = 64;
const int FRAMES_PER_SECOND = 100;
 
/**********************
 * Global declarations
 **********************/

int buttonUpPress_Cnt = 0;
int buttonUpState = 0;
int prevButtonUpState = 0;
int count = 0;
int buttonDnState = 0;
int prevButtonDnState = 0;
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };
CRGB leds[NUM_LEDS];
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

/************************
 * Function declarations
 ************************/
void rainbow();
void rainbowWithGlitter();
void addGlitter(fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
void pink();
void pinkWithGlitter();
void blue();
void blueWithGlitter();
void red();
void redWithGlitter();
void green();
void greenWithGlitter();
void gold();
void goldWithGlitter();
void redGreen();
void redGreenWithGlitter();
void pride();
void pacifica();
void black();
void redBlue();
void redBlueWithGlitter();
void greenBlue();
void greenBlueWithGlitter();
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff);
void pacifica_add_whitecaps();
void pacifica_deepen_colors();
void redBlueWhiteYellowGreen();
void redBlueWhiteYellowGreenWithGlitter();

/*************************************
 * List of patterns to cycle through.
 *************************************/

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { 
                                black,
                                rainbow,
                                rainbowWithGlitter,
                                confetti,
                                sinelon,
                                juggle,
                                bpm,
                                pink,
                                pinkWithGlitter,
                                blue,
                                blueWithGlitter,
                                red,
                                redWithGlitter,
                                green,
                                greenWithGlitter,
                                gold,
                                goldWithGlitter,
                                redGreen,
                                redGreenWithGlitter,
                                pride,
                                pacifica,
                                redBlue,
                                redBlueWithGlitter,
                                greenBlue,
                                greenBlueWithGlitter,
                                redBlueWhiteYellowGreen,
                                redBlueWhiteYellowGreenWithGlitter
                              };

/************************************
 * List of names for the LCD display
 ************************************/
String PatternNameRow0[] = {
                          "Turn Off",
                          "Rainbow",
                          "Rainbow",
                          "Confetti",
                          "Sweep",
                          "Juggle",
                          "Beats per",
                          "Pink",
                          "Pink",
                          "Blue",
                          "Blue",
                          "Red",
                          "Red",
                          "Green",
                          "Green",
                          "Gold",
                          "Gold",
                          "Red and Green",
                          "Red and Green",
                          "Pride",
                          "Ocean Waves",
                          "Red and Blue",
                          "Red and Blue",
                          "Green and Blue",
                          "Green and Blue",
                          "Red,Blue,White,",
                          "Red,Blue,White,"
                         };

String PatternNameRow1[] = {
                          " ",              // Turn Off
                          " ",              // Rainbow
                          " with Glitter",  // Rainbow
                          " ",              // Confetti
                          " ",              // Sweep
                          " ",              // Juggle
                          " Minute",        // Beats per
                          " ",              // Pink
                          " with Glitter",  // Pink
                          " ",              // Blue
                          " with Glitter",  // Blue
                          " ",              // Red
                          " with Glitter",  // Red
                          " ",              // Green
                          " with Glitter",  // Green
                          " ",              // Gold
                          " with Glitter",  // Gold
                          " ",              // Red and Green
                          " with Glitter",  // Red and Green
                          " ",              // Pride
                          " ",              // Ocean Waves
                          " ",              // Red and Blue
                          " with Glitter",  // Red and Blue
                          " ",              // Green and Blue
                          " with Glitter",  // Green and Blue
                          " Yellow & Green",
                          " Yell&GrnGlitter"
                         };

/***********
 * setup()
 ***********/
 
void setup() {
    Serial.begin(9600);
    Serial.println("Button Count:");

    pinMode(buttonUpPin, INPUT_PULLUP);
    pinMode(buttonDnPin, INPUT_PULLUP);

    // Set display type as 16 char, 2 rows
    lcd.begin(16,2);
  
    // Print on first row
    lcd.setCursor(0,0);
    lcd.print("Here we go");
  
    // Print on second row
    lcd.setCursor(0,1);
    lcd.print(" ");

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
}

/***********
 * loop()
 ***********/
 
void loop() {
    buttonUpState = digitalRead(buttonUpPin);
    buttonDnState = digitalRead(buttonDnPin);
    
    if (buttonUpState != prevButtonUpState) {
      if (buttonUpState == HIGH) {
    } else {
            count++;
            if (count >= MAX_NUM) count = 0;
            Serial.print(count);
            Serial.print(" - ");
            Serial.print(PatternNameRow0[count]);
            Serial.println(PatternNameRow1[count]);
            lcd.clear();
            lcd.setCursor(0,0);
            delay(100);
            lcd.print(PatternNameRow0[count]);
            delay(100);
            lcd.setCursor(0,1);
            delay(100);
            lcd.print(PatternNameRow1[count]);
        }
     delay(50);
    }
     prevButtonUpState = buttonUpState;

    if (buttonDnState != prevButtonDnState) {
      if (buttonDnState == HIGH) {
    } else {
            count--;
            if (count < 0)  count = MAX_NUM - 1;
            Serial.print(count);
            Serial.print(" - ");
            Serial.print(PatternNameRow0[count]);
            Serial.println(PatternNameRow1[count]);
            lcd.clear();
            lcd.setCursor(0,0);
            delay(100);
            lcd.print(PatternNameRow0[count]);
            delay(100);
            lcd.setCursor(0,1);
            delay(100);
            lcd.print(PatternNameRow1[count]);
        }
        delay(50);
    }
      prevButtonDnState = buttonDnState;

      // Call the current pattern function once, updating the 'leds' array
      gPatterns[count]();

      // send the 'leds' array out to the actual LED strip
      FastLED.show();

      // insert a delay to keep the framerate modest
      FastLED.delay(1000/FRAMES_PER_SECOND);

      // do some periodic updates
      EVERY_N_MILLISECONDS(20)
      {
          gHue++;
      }   // slowly cycle the "base color" through the rainbow
}

/************
 * Functions
 ************/
 
void fadeall()
{
  for (int i = 0; i < NUM_LEDS; i++)
      leds[i].nscale8(250);
}

/***************
 * LED Patterns
 ***************/

void rainbow()
{
    // FastLED's built-in rainbow generator

    fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter

    rainbow();
    addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly

    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails

    fadeToBlackBy(leds, NUM_LEDS, 20);
    int pos = beatsin16(13, 0, NUM_LEDS - 1);
    leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)

    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    
    for (int i = 0; i < NUM_LEDS; i++)
    { // 9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle()
{
    // eight colored dots, weaving in and out of sync with each other

    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

void pink()
{
    for (int i = 0; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::DeepPink;
    for (int i = 1; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::MediumVioletRed;
}

void pinkWithGlitter()
{
    pink();
    addGlitter(80);
}

void blue()
{
    for (int i = 0; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::DeepSkyBlue;
    for (int i = 1; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::DarkBlue;
        
}

void blueWithGlitter()
{
    blue();
    addGlitter(80);
}

void red()
{
    for (int i = 0; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Red;
    for (int i = 1; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::DarkRed;
    for (int i = 2; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Maroon;
}

void redWithGlitter()
{
    red();
    addGlitter(80);
}

void green()
{
    for (int i = 0; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::DarkGreen;
    for (int i = 1; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::Chartreuse;
    for (int i = 2; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::ForestGreen;
    for (int i = 3; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::Green;
    for (int i = 4; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::LawnGreen;

}

void greenWithGlitter()
{
    green();
    addGlitter(80);
}

void gold()
{
    for (int i = 0; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Gold;
    for (int i = 1; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Yellow;
     for (int i = 2; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Orange;
}

void goldWithGlitter()
{
    gold();
    addGlitter(80);
}

void redGreen()
{
    for (int i = 0; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::Red;
    for (int i = 1; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::DarkRed;
    for (int i = 2; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::Maroon;
    for (int i = 3; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::DarkGreen;
    for (int i = 4; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::Chartreuse;
    for (int i = 5; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::ForestGreen;
}

void redGreenWithGlitter()
{
    redGreen();
    addGlitter(80);
}

void boing() { 
  static uint8_t hue = 0;
  
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++)
  {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
}

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--)
  {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

void prideDisplay() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

void pride()
{
  prideDisplay();
}

void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}

void pacifica()
{
    pacifica_loop();
}

void black()
{
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB::Black;
}

void redBlue()
{
    for (int i = 0; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::Red;
    for (int i = 1; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::Blue;
}

void redBlueWithGlitter()
{
    redBlue();
    addGlitter(80);
}

void greenBlue()
{
    for (int i = 0; i < NUM_LEDS; i += 2) {
        leds[i] = CRGB::DarkBlue;
    }
    for (int i = 1; i < NUM_LEDS; i += 2) {
        leds[i] = CRGB::DarkGreen;
    }
}

void greenBlueWithGlitter()
{
    greenBlue();
    addGlitter(80);
}    

void redBlueWhiteYellowGreen()
{
    for (int i = 0; i < NUM_LEDS; i += 5) {
        leds[i] = CRGB::Red;
    }
    for (int i = 1; i < NUM_LEDS; i += 5) {
        leds[i] = CRGB::Blue;
    }
    for (int i = 2; i < NUM_LEDS; i += 5) {
        leds[i] = CRGB::White;
    }
    for (int i = 3; i < NUM_LEDS; i += 5) {
        leds[i] = CRGB::Yellow;
    }
    for (int i = 4; i < NUM_LEDS; i += 5) {
        leds[i] = CRGB::Green;
    }
}


void redBlueWhiteYellowGreenWithGlitter()
{
    redBlueWhiteYellowGreen();
    addGlitter(80);
}
