#ifndef PATTERNS_H
#define PATTERNS_H

#include "palette.h"
#include "FastLED.h"

#define MAX_POSSIBLE_SPEED 1<<12

struct PatternSettings {
  uint8_t numLeds;
  int32_t initSpeed;
  int32_t maxSpeed;
  int32_t minSpeed;
  int32_t acceleration;
  uint16_t colIncrement;
  uint8_t eventProb;
  uint16_t eventLength;
  uint8_t groupSize;
  
  PatternSettings(int _numLeds, int _initSpeed, int _maxSpeed, int _minSpeed, int _acceleration, int _colIncrement, int _eventProb,
                  int _eventLength, int _groupSize) : numLeds(_numLeds), initSpeed(_initSpeed), maxSpeed(_maxSpeed), minSpeed(_minSpeed), acceleration(_acceleration),
                  colIncrement(_colIncrement), eventProb(_eventProb), eventLength(_eventLength), groupSize(_groupSize) {}
};

class PatternState {
public:
  PatternState(const PatternSettings& settings) : m_settings(settings), m_currLed(0), m_currColorIndex(0),
    m_currSpeed(settings.initSpeed), m_currAcceleration(settings.acceleration) {}
  
  const PatternSettings& settings() { return m_settings; }
  int& currLed() { return m_currLed; }
  int& currColorIndex() { return m_currColorIndex; }
  int direction() { return m_currSpeed >= 0 ? 1 : -1; }
  
  int currDelay() {
    p("sp="); p(m_currSpeed); p("\n");
    return map(abs(m_currSpeed), 0, MAX_POSSIBLE_SPEED, 1000, 1);
  }
  
  void update() {
    int64_t newSpeed = m_currSpeed + m_currAcceleration * currDelay();
    if (newSpeed > m_settings.maxSpeed || newSpeed < m_settings.minSpeed) {
      m_currAcceleration *= -1;
      newSpeed = m_currSpeed + m_currAcceleration * currDelay();
    }
    m_currSpeed = newSpeed;
    m_currColorIndex += m_settings.colIncrement;
  }
  
private:
  PatternSettings m_settings;
  int m_currLed;
  int m_currColorIndex;
  int32_t m_currSpeed;
  int32_t m_currAcceleration;
};

class Pattern {
public:
  Pattern(CRGB* leds, Palette palette, PatternSettings settings) 
    : m_leds(leds), m_palette(palette), m_state(settings) {}
    
  void setPalette(Palette palette) {
    m_palette = palette;
  }

  // show a spinning gradient that can change direction
  // uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb
  void gradient();

  // show a spinning wave that can change direction
  // uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
  void wave();

  // show a spinning particle that can change direction
  // uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
  void particle();

  // show randomly appearing sparkles
  // uses: initSpeed, colIncrement, eventProb, eventLength, groupSize
  template<int NUM_LEDS>
  void sparkle();

  // randomly jitter back and forth around the circle
  // uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
  void randomWalk();

  // pulse the whole strip at a dynamic frequency
  // uses: initSpeed, maxSpeed, acceleration, colIncrement, eventProb, groupSize
  void pulse();
    
private:
  void gradient(bool isWave, bool singleWave);

  CRGB* m_leds;
  Palette m_palette;
  PatternState m_state;
};

#endif

