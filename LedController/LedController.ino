#include "pattern.h"
#include "palette.h"

#include "FastLED.h"
#include <Wire.h>

#define IR_CMD_UP 0x9A
#define IR_CMD_LEFT 0x3B
#define IR_CMD_DOWN 0x73
#define IR_CMD_RIGHT 0x05
#define IR_CMD_ENTER 0x24
#define IR_CMD_EXIT 0xCD

#define PALETTE_SIZE 1024
#define PALETTE_CHANGE_MS 10000
#define NUM_PALETTES (sizeof(colorTable) / 6)

#define NUM_LEDS 60
#define DATA_PIN 10
CRGB leds[NUM_LEDS];

//uint32_t lastPaletteChangeTime = millis();

//PatternSettings order: numLeds, initSpeed, maxSpeed, minSpeed, acceleration, colIncrement, eventProb, eventLength, particleSize
Pattern gradient(leds, Palette(1, PALETTE_SIZE), PatternSettings(NUM_LEDS, 1<<11, 1<<12, 1<<4, 1<<3, 25, 0, 0, 0));

void setup() {
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(38400);           // start serial for output
  
  pinMode(13, OUTPUT);

  delay(2000);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(32);
  Serial.println("Setup");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(Wire.available() > 0)
  {
    byte code = Wire.read(); // receive byte as a character
    Serial.println(code, HEX);        // print the character

    if (code == IR_CMD_ENTER)
      gradient.setPalette(Palette(rand() % NUM_PALETTES, PALETTE_SIZE));
    
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
  }
}

void loop() {
//  for (int i = 0; i < NUM_LEDS; i++) {
//    if (color == 0)
//      leds[i] = CRGB::Red;
//    else if (color == 1)
//      leds[i] = CRGB::Green;
//    else
//      leds[i] = CRGB::Blue;
//      
//    FastLED.delay(50);
//    FastLED.show();
//    leds[i] = CRGB::Black; 
//  }

  gradient.gradient();
    
  // change the palette on a regular interval
//  if (millis() - lastPaletteChangeTime > PALETTE_CHANGE_MS) {
//    gradient.setPalette(Palette(rand() % NUM_PALETTES, PALETTE_SIZE));
//    lastPaletteChangeTime = millis();
//  }
}
