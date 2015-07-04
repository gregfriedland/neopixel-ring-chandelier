#include "pattern.h"

#define SINETABLE_MAX_IN ((1<<16)-1)
#define SINETABLE_MAX_OUT ((1<<15)-1)
#define MAX_COLOR 256

void PatternState::update() {
  accel_t accelDirection = rand() % 2 == 0 ? 1 : -1;
  speed_t newSpeed = m_currSpeed * (MAX_ACCELERATION + accelDirection * m_currAcceleration) / MAX_ACCELERATION;
  newSpeed = max(min(newSpeed, m_settings.maxSpeed), m_settings.minSpeed);

  m_currSpeed = newSpeed;
  m_currColorIndex = (m_currColorIndex + m_settings.colIncrement) % PALETTE_SIZE;
//p("sp="); p(m_currSpeed); p(" col="); p(m_currColorIndex); p("\n");	  
}


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
  ledind_t waveSize = m_state.settings().groupSize;

  for (ledind_t i = 0; i < m_state.settings().numLeds; i++) {
    colind_t colIndex = (m_state.pos() / POS_PRECISION + i) * m_state.settings().colIncrement;
    CRGB col = m_palette.getColor(colIndex);
    //p("i="); p(i); p(" ci="); p(colIndex); p(" c="); p(col, HEX); p("\n");
    m_leds[i] = col;
    
   if (isWave) {
     int16_t waveVal;
     ledind_t waveIndex = m_state.pos() / POS_PRECISION + i;
     if (waveIndex >= waveSize && singleWave)
       waveVal = 0;
     else
       waveVal = sin16((waveIndex % waveSize) * SINETABLE_MAX_IN / waveSize);

     // scale intensity with number between 0-255
     m_leds[i] %= waveVal * MAX_COLOR / SINETABLE_MAX_OUT;
   }
  }
  
  // FIX: make sure we wrap around smoothly?
  FastLED.show();
  FastLED.delay(FRAME_DELAY);
  m_state.update();
  m_state.pos() = (m_state.pos() + m_state.currSpeed()) % (POS_PRECISION * NUM_LEDS);
}

// show randomly appearing sparkles
// uses: initSpeed, colIncrement, eventProb, eventLength, groupSize
void Pattern::sparkle() {
  struct Sparkle {
    Sparkle() : rgb(0), direction(true), val(0) {}
    CRGB rgb;
    bool direction;
    uint8_t val;
  };
  
  static Sparkle sparkles[NUM_LEDS];
    
  // randomly start new sparkles
  if (rand() % PROB_MAX < m_state.settings().eventProb) {
    ledind_t sparkleIndex = rand() % m_state.settings().numLeds;
    if (sparkles[sparkleIndex].val == 0) {
      sparkles[sparkleIndex].rgb = m_palette.getColor(m_state.currColorIndex());
      sparkles[sparkleIndex].direction = true;
      sparkles[sparkleIndex].val = 1;
    }
  }
  
  for (ledind_t i = 0; i < m_state.settings().numLeds; i++) {
    // set leds from sparkles
    if (sparkles[i].val > 0) {
      m_leds[i] = sparkles[i].rgb;
      m_leds[i] %= sparkles[i].val * PROB_MAX / m_state.settings().eventLength;
    
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
  FastLED.delay(FRAME_DELAY);
  m_state.update();
}

void Pattern::randomWalk() {
  ledind_t groupSize = m_state.settings().groupSize;
  for (ledind_t i = 0; i < m_state.settings().numLeds; i++) {
    ledind_t ledIndex = m_state.pos() / POS_PRECISION + i;
    m_leds[ledIndex % m_state.settings().numLeds] = m_palette.getColor(ledIndex * m_state.settings().colIncrement);
      
    int16_t val;
    if (i >= groupSize)
      val = 0;
    else
      val = sin16((i % groupSize) * SINETABLE_MAX_IN / groupSize);

    //  scale intensity with number between 0-255
    m_leds[ledIndex % m_state.settings().numLeds] %= val * MAX_COLOR / SINETABLE_MAX_OUT;
  }
  
  // used rand to decideif we move at all and move forward and backwards 50% of the ttime
  if (rand() % PROB_MAX < m_state.settings().eventProb)
    m_state.pos() += rand() % 2 == 0 ? 1 : -1;

  FastLED.show();
  FastLED.delay(FRAME_DELAY);
  m_state.update();
}

// pulse the whole strip at a dynamic frequency
// uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
void Pattern::pulse() {
  CRGB rgb = m_palette.getColor(m_state.currColorIndex());
  int16_t val = sin16(m_state.currColorIndex() % (SINETABLE_MAX_IN / 2)); // FIX: probably repeat faster than this

  for (ledind_t i = 0; i < m_state.settings().numLeds; i++) {
    m_leds[i] = rgb;
    m_leds[i] %= val * MAX_COLOR / SINETABLE_MAX_OUT;
  }

  FastLED.show();
  FastLED.delay(FRAME_DELAY);
  m_state.update();
}

