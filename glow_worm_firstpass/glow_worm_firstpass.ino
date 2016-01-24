#include "FastLED.h"

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define PIR_PIN_1   6
#define PIR_PIN_2   7
#define PIR_PIN_3   8
#define PIR_PIN_4   9
#define LEDS_PIN_1    10
#define LEDS_PIN_2    11
#define LEDS_PIN_3    12
#define LEDS_PIN_4    13
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    4
#define NUM_LEDS_PER_STRIP 1
#define NUM_PIRS    4
CRGB leds[NUM_LEDS];
int ledhues[NUM_LEDS];
int ledvals[NUM_LEDS];
int pir_pins[NUM_PIRS];
int pir_states[NUM_PIRS];
int stimulated_pirs[NUM_PIRS];
int MAX_PIXEL_HUE_DIFF=20;
int MAX_PIXEL_VAL_DIFF=20;
int randomaddhue=0;
int randomaddsat=0;





#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {

  //Manually define which pins are at which pirs
  pir_pins[0]=PIR_PIN_1;
  pir_pins[1]=PIR_PIN_2;
  pir_pins[2]=PIR_PIN_3;
  pir_pins[3]=PIR_PIN_4;

  for (int i = 0; i < NUM_PIRS; ++i)
  {
    stimulated_pirs[i] = 0; //goes 0-255
    pir_states[i] = LOW; //drop it drop it low
    pinMode(pir_pins[i], INPUT);
  }
  
  delay(3000); // 3 second delay for recovery
  for (int i = 0; i < NUM_LEDS; i++) {
    ledhues[i]=180;
    ledvals[i]=200;
  }
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LEDS_PIN_1,COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LEDS_PIN_2,COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LEDS_PIN_3,COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LEDS_PIN_4,COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  for (int i; i < NUM_PIRS; ++i)
  {
    pir_states[i]=digitalRead(pir_pins[i]);
  }
  // Call the current pattern function once, updating the 'leds' array
  //gPatterns[gCurrentPatternNumber]();
  //random16_add_entropy( random());
  assess_excitement();
  update_blinkies_based_on_excitement();
  
  //AlexFire(); // run simulation frame
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
 // EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
 // EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


void assess_excitement()
{
  for (int i = 0; i < NUM_PIRS; ++i)
  {
    if (pir_states[i]==HIGH && stimulated_pirs[i]<255)
    {
      stimulated_pirs[i] = stimulated_pirs[i]+1; //goes 0-255
    }
    else 
    {
      stimulated_pirs[i] = stimulated_pirs[i]-1; //goes 0-255
    }
  }
}


void update_blinkies_based_on_excitement()
{
  for (int i = 0; i < NUM_PIRS; ++i)
  {
    leds[i]=CHSV(180,255,stimulated_pirs[i]);
  }
}


void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

//H value: 160-210
//S value: 50-255
void AlexFire()
{
  for (int i = 0; i < NUM_LEDS; i++) {
      randomaddhue=(randomaddhue+3*random(-22,20))/4;
      randomaddsat=(randomaddsat+3*random(-11,12))/4;
      ledhues[i]=ledhues[i]+randomaddhue;
      
      //keep the colors within the yellow-red spectrum
        if (ledhues[i]>210){
          ledhues[i]=ledhues[i]-2*randomaddhue;
        }
        if (ledhues[i]<155){
          ledhues[i]=ledhues[i]-2*randomaddhue;
        }
      
      
      //make sure the colors don't differ too much pixel-to-pixel
      if (i > 2) { //bla bla bla indexing waaaaaaaah
          if (abs(ledhues[i]-ledhues[i-1])>MAX_PIXEL_HUE_DIFF){
            if (ledhues[i]>ledhues[i-1]){
              ledhues[i]=ledhues[i-1]+MAX_PIXEL_HUE_DIFF;
          }
            if (ledhues[i]<ledhues[i-1]){
              ledhues[i]=ledhues[i-1]-MAX_PIXEL_HUE_DIFF;
          }
        }
      }
      
      
      ledvals[i]=ledvals[i]+randomaddsat;
      
       if (ledvals[i]>190){
          ledvals[i]=ledvals[i]-2*randomaddsat;
        }
        if (ledhues[i]<70){
          ledvals[i]=ledvals[i]-2*randomaddsat;
        }
      
      //keep continuity in brightnesses
      if (i > 2) { //bla bla bla indexing waaaaaaaah
          if (abs(ledvals[i]-ledvals[i-1])>MAX_PIXEL_VAL_DIFF){
            if (ledvals[i]>ledvals[i-1]){
              ledvals[i]=ledvals[i-1]+MAX_PIXEL_VAL_DIFF;
          }
            if (ledvals[i]<ledvals[i-1]){
              ledvals[i]=ledvals[i-1]-MAX_PIXEL_VAL_DIFF;
          }
        }
      }
      
      leds[i] = CHSV( ledhues[i], 255, ledvals[i]);
    }
    leds[0]=CHSV(180,255,192);
}


void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = HeatColor( heat[j]);
    }
}


void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
