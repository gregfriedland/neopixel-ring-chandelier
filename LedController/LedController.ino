#include "FastLED.h"
#include <Wire.h>

#include "settings.h"
#include "pattern.h"
#include "palette.h"

CRGB leds[NUM_LEDS];

//uint32_t lastPaletteChangeTime = millis();

//PatternSettings order: numLeds, initSpeed, maxSpeed, minSpeed, acceleration, colIncrement, eventProb, eventLength, particleSize
Pattern gradient(leds, Palette(2, PALETTE_SIZE), PatternSettings(NUM_LEDS, 100, MAX_SPEED, 100, MAX_ACCELERATION/10, 
	PALETTE_SIZE / NUM_LEDS / 2, 0, 0, 0));

void setup() {
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  
  pinMode(13, OUTPUT);

  delay(2000);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(64);
  FastLED.setCorrection(TypicalSMD5050);

  Serial.println("Setup");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(Wire.available() > 0)
  {
    byte code = Wire.read(); // receive byte as a character
    Serial.print("Remote recv: "); Serial.println(code, HEX);

    if (code == IR_CMD_ENTER)
      gradient.setPalette(Palette(rand() % NUM_PALETTES, PALETTE_SIZE));
    
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
  }
}

void outputFPS() {
  static uint32_t lastFPSCalcTime = millis();
  static int frameCounter = 0;

  frameCounter++;
  if (frameCounter > 500) {
    Serial.print(frameCounter * 1000UL / (millis() - lastFPSCalcTime));
    Serial.println(" fps");
    lastFPSCalcTime = millis();
    frameCounter = 0;
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

  outputFPS();
}
