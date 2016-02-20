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
    stimulated_pirs[i] = 255; //goes 0-255
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

  // Begin serial dump for debug
  Serial.begin(9600);
}


void loop()
{
  for (int i = 0; i < NUM_PIRS; ++i)
  {
    pir_states[i] = digitalRead(pir_pins[i]);
  }
  assess_excitement();
  update_blinkies_based_on_excitement();
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void assess_excitement()
{
  for (int i = 0; i < NUM_PIRS; ++i)
  {
    Serial.println(pir_states[i] + stimulated_pirs[i]);
    
    if (pir_states[i] == HIGH && stimulated_pirs[i] > 0)
    {
      stimulated_pirs[i] = stimulated_pirs[i] - 1; //goes 0-255
    }
    else if (pir_states[i] == LOW && stimulated_pirs[i] < 255)
    {
       stimulated_pirs[i] = stimulated_pirs[i] + 1; //goes 0-255
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



