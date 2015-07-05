#include "FastLED.h"
#include <Wire.h>

#include "settings.h"
#include "pattern.h"
#include "palette.h"

CRGB leds[NUM_LEDS];

//uint32_t lastPaletteChangeTime = millis();

//PatternSettings order: numLeds, initSpeed, maxSpeed, minSpeed, acceleration, colIncrement, eventProb, eventLength, groupSize
Pattern pattern(leds, Palette(120, PALETTE_SIZE), PatternSettings(NUM_LEDS, 500, 3000, 500, MAX_ACCELERATION/10, 
	PALETTE_SIZE / NUM_LEDS / 2, 0, 0, 16));

typedef enum { GRADIENT, WAVE, PARTICLE, NUM_MODES } PatternMode;

PatternMode patternMode = WAVE;


void setup() {
  Wire.begin(4);                  // join i2c bus with address #4
  Wire.onReceive(receiveEvent);   // register event
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

    if (code == IR_CMD_ENTER) {
      pattern.setPalette(Palette(rand() % NUM_PALETTES, PALETTE_SIZE));
      Serial.print("New palette: "); Serial.println(pattern.palette().index());
    } else if (code == IR_CMD_UP) {
      pattern.state().settings().minSpeed *= 1.2;
      pattern.state().settings().maxSpeed *= 1.2;
      Serial.print("New max speed: "); Serial.println(pattern.state().settings().maxSpeed);
    } else if (code == IR_CMD_DOWN) {
      pattern.state().settings().minSpeed *= 0.8;
      pattern.state().settings().maxSpeed *= 0.8;
      Serial.print("New max speed: "); Serial.println(pattern.state().settings().maxSpeed);
    } else if (code == IR_CMD_LEFT) {
      patternMode = (PatternMode) (((int)patternMode - 1 + NUM_MODES) % NUM_MODES);
      Serial.print("New pattern: "); Serial.println(patternMode);
    } else if (code == IR_CMD_RIGHT) {
      patternMode = (PatternMode) (((int)patternMode + 1) % NUM_MODES);
      Serial.print("New pattern: "); Serial.println(patternMode);
    } else if (code == IR_CMD_0) {
      FastLED.setBrightness(0);
    } else if (code == IR_CMD_1) {
      FastLED.setBrightness(2);
    } else if (code == IR_CMD_2) {
      FastLED.setBrightness(4);
    } else if (code == IR_CMD_3) {
      FastLED.setBrightness(8);
    } else if (code == IR_CMD_4) {
      FastLED.setBrightness(16);
    } else if (code == IR_CMD_5) {
      FastLED.setBrightness(32);
    } else if (code == IR_CMD_6) {
      FastLED.setBrightness(64);
    } else if (code == IR_CMD_7) {
      FastLED.setBrightness(128);
    } else if (code == IR_CMD_8) {
      FastLED.setBrightness(255);
    }
    
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

  switch(patternMode) {
    case GRADIENT:
      pattern.gradient();
      break;
    case WAVE:
      pattern.wave();
      break;
    case PARTICLE:
      pattern.particle();
      break;
  }

    
  // change the palette on a regular interval
//  if (millis() - lastPaletteChangeTime > PALETTE_CHANGE_MS) {
//    gradient.setPalette(Palette(rand() % NUM_PALETTES, PALETTE_SIZE));
//    lastPaletteChangeTime = millis();
//  }

  outputFPS();
}
