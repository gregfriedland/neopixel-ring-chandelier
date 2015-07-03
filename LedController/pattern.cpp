#include "pattern.h"

#define MAX_UINT16 ((2^16)-1)
#define MAX_INT16 ((2^15)-1)
#define PROB_MAX 256

// show a spinning gradient that can change direction
// uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb
void Pattern::gradient() {
  gradient(false, false);
}

// show a spinning wave that can change direction
// uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
void Pattern::wave() {
  gradient(true, false);
}

// show a spinning wave that can change direction
// uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
void Pattern::particle() {
  gradient(true, true);
}

void Pattern::gradient(bool isWave, bool singleWave) {
  int waveSize = m_state.settings().groupSize;

  for (int i = 0; i < m_state.settings().numLeds; i++) {
    int colIndex = (m_state.currLed() + m_state.direction() * i) * m_state.settings().colIncrement;
    CRGB col = m_palette.getColor(colIndex);
    p("i="); p(i); p(" ci="); p(colIndex); p(" c="); p(col, HEX); p("\n");
    m_leds[i] = col;
    
//    if (isWave) {
//      int waveVal;
//      int waveIndex = m_state.currLed() + i;
//      if (waveIndex >= waveSize && singleWave)
//        waveVal = 0;
//      else
//        waveVal = sin16((waveIndex % waveSize) * MAX_UINT16 / waveSize);
//
//      //  scale intensity with number between 0-255
//      m_leds[i] %= waveVal * 256 / MAX_INT16;
//    }
  }
  
  // FIX: make sure we wrap around smoothly?
  FastLED.show();
  int currDelay = m_state.currDelay();
  p("del="); p(currDelay); p("\n");
  FastLED.delay(currDelay);
  //m_state.update();
  m_state.currLed() = (m_state.currLed() + 1) % m_state.settings().numLeds; // fix allow backwards moves?
}

// show randomly appearing sparkles
// uses: initSpeed, colIncrement, eventProb, eventLength, groupSize
template<int NUM_LEDS>
void Pattern::sparkle() {
  struct Sparkle {
    Sparkle() : rgb(0), direction(true), val(0) {}
    CRGB rgb;
    bool direction;
    int val;
  };
  
  static Sparkle sparkles[NUM_LEDS];
    
  // randomly start new sparkles
  if (rand() % PROB_MAX < m_state.settings().eventProb) {
    int sparkleIndex = rand() % m_state.settings().numLeds;
    if (sparkles[sparkleIndex].val == 0) {
      sparkles[sparkleIndex].rgb = m_palette.getColor(m_state.currColorIndex());
      sparkles[sparkleIndex].direction = true;
      sparkles[sparkleIndex].val = 1;
    }
  }
  
  for (int i = 0; i < m_state.settings().numLeds; i++) {
    // set leds from sparkles
    if (sparkles[i].val > 0) {
      m_leds[i] = sparkles[i].rgb;
      m_leds[i] %= sparkles[i].val * 256 / m_state.settings().eventLength;
    
      // adjust sparkles
      if (sparkles[i].direction) {
        sparkles[i].val++;
        if (sparkles[i].val >= m_state.settings().eventLength) {
          sparkles[i].direction = false;
        }
      } else {
        sparkles[i].val--;
      }
    }
  }
  
  FastLED.show();
  FastLED.delay(m_state.currDelay());
  m_state.update();
}

void Pattern::randomWalk() {
  int groupSize = m_state.settings().groupSize;
  for (int i = 0; i < m_state.settings().numLeds; i++) {
    int ledIndex = m_state.currLed() + i;
    m_leds[ledIndex % m_state.settings().numLeds] = m_palette.getColor(ledIndex * m_state.settings().colIncrement);
      
    int val;
    if (i >= groupSize)
      val = 0;
    else
      val = sin16((i % groupSize) * MAX_UINT16 / groupSize);

    //  scale intensity with number between 0-255
    m_leds[ledIndex % m_state.settings().numLeds] %= val * 256 / MAX_INT16;
  }
  
  // used rand to decideif we move at all and move forward and backwards 50% of the ttime
  if (rand() % PROB_MAX < m_state.settings().eventProb)
    m_state.currLed() = (m_state.currLed() + rand() % 2 == 0 ? 1 : -1) % m_state.settings().numLeds;

  FastLED.show();
  FastLED.delay(m_state.currDelay());
  m_state.update();
}

// pulse the whole strip at a dynamic frequency
// uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
void Pattern::pulse() {
  CRGB rgb = m_palette.getColor(m_state.currColorIndex());
  int16_t val = sin16(m_state.currColorIndex() % (MAX_UINT16 / 2)); // FIX: probably repeat faster than this

  for (int i = 0; i < m_state.settings().numLeds; i++) {
    m_leds[i] = rgb;
    m_leds[i] %= val * 256 / MAX_INT16;
  }

  FastLED.show();
  FastLED.delay(m_state.currDelay());
  m_state.update();
}

