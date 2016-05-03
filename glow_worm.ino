#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define PIR_PIN_0   6
#define PIR_PIN_1   7
#define PIR_PIN_2   8
#define PIR_PIN_3   9
#define LEDS_PIN_0    13
#define LEDS_PIN_1    12
#define LEDS_PIN_2    11
#define LEDS_PIN_3    10
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_PIRS    4
#define NUM_LEDS    1
#define NUM_LED_STRIPS 4
CRGB leds[NUM_LEDS];
CLEDController *controllers[NUM_LED_STRIPS];

int ledhues[NUM_LEDS];
int ledvals[NUM_LEDS];
int led_pins[NUM_LEDS];
int pir_pins[NUM_PIRS];
int pir_states[NUM_PIRS];
int stimulated_pirs[NUM_PIRS];
int MAX_PIXEL_HUE_DIFF=20;
int MAX_PIXEL_VAL_DIFF=20;
int randomaddhue=0;
int randomaddsat=0;

#define gBRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {

  //Manually define which pins are at which pirs
  pir_pins[0]=PIR_PIN_0;
  pir_pins[1]=PIR_PIN_1;
  pir_pins[2]=PIR_PIN_2;
  pir_pins[3]=PIR_PIN_3;
  
  // tell FastLED about the LED strip configuration
  controllers[0] = &FastLED.addLeds<LED_TYPE,LEDS_PIN_0>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<LED_TYPE,LEDS_PIN_1>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<LED_TYPE,LEDS_PIN_2>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  controllers[3] = &FastLED.addLeds<LED_TYPE,LEDS_PIN_3>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  for (int i = 0; i < NUM_PIRS; ++i)
  {
    stimulated_pirs[i] = 255; //goes 0-255
    pir_states[i] = LOW; //drop it drop it low
    pinMode(pir_pins[i], INPUT);
  }
  
  delay(3000); // 3 second delay for recovery
  for (int i = 0; i < NUM_LEDS; i++) {
    //set the leds
    leds[i]=CHSV(200,255,stimulated_pirs[i]);
    ledhues[i]=180;
    ledvals[i]=200;
  }
  
  // Begin serial dump for debug
  Serial.begin(9600);
}


void loop()
{
  for (int i = 0; i < NUM_PIRS; ++i)
  {
    pir_states[i] = digitalRead(pir_pins[i]); // get state of infrared
    Serial.println(i);
    Serial.println(pir_states[i]);
    Serial.println(stimulated_pirs[i]); //debug, print to serial

    /* ===============================
     *  Patterns for life
     * ===============================
     * If movement sensed then retreat!
     * pir_states go from 0 off to 255 full brightness
     * Randomly fade to show activity, glitch out in their matrix
     */

     /* ===============================
      * pir_states
      * ===============================
      */
    if (pir_states[i] == HIGH && stimulated_pirs[i] > 128)
    {
      stimulated_pirs[i] = stimulated_pirs[i] - 7;
      controllers[i]->showLeds(stimulated_pirs[i]);
    }
    else if (pir_states[i] == HIGH && stimulated_pirs[i] > 20)
    {
      stimulated_pirs[i] = stimulated_pirs[i] - 1;
      controllers[i]->showLeds(stimulated_pirs[i]);
    }
    else if (pir_states[i] == LOW && stimulated_pirs[i] < 128)
    {
       stimulated_pirs[i] = stimulated_pirs[i] + 4;
       controllers[i]->showLeds(stimulated_pirs[i]);
    }
    else if (pir_states[i] == LOW && stimulated_pirs[i] < 255)
    {
       stimulated_pirs[i] = stimulated_pirs[i] + 1;
       controllers[i]->showLeds(stimulated_pirs[i]);
    }
    
    /*===============================
     * Random activity
     * ===============================
     */
    if( random8() < 80) {
        leds[i]=CHSV(200,255,stimulated_pirs[i]); //randomly turn light brightness back to state
    }
    if( random8() < 20 && stimulated_pirs[i] > 30) {
        controllers[i]->showLeds(stimulated_pirs[i] - 20); //randomly turn light brightness off for glitch
    }
    if( random8() < 20 && stimulated_pirs[i] > 60) {
        leds[i]=CHSV(200,255,stimulated_pirs[i] - 50); //randomly turn light brightness off for glitch
    }
  }
  delay(30);
  
  
}



