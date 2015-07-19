#ifndef PATTERNS_H
#define PATTERNS_H

#include "FastLED.h"
#include "palette.h"
#include "settings.h"

struct PatternSettings {
  ledind_t numLeds;
  speed_t initSpeed;
  speed_t maxSpeed;
  speed_t minSpeed;
  accel_t acceleration;
  colind_t colIncrement;
  prob_t eventProb;
  event_t eventLength;
  ledind_t groupSize;
  
  PatternSettings(ledind_t _numLeds, speed_t _initSpeed, speed_t _maxSpeed, speed_t _minSpeed, accel_t _acceleration, colind_t _colIncrement, prob_t _eventProb,
                  event_t _eventLength, ledind_t _groupSize) : numLeds(_numLeds), initSpeed(_initSpeed), maxSpeed(_maxSpeed), minSpeed(_minSpeed), acceleration(_acceleration),
                  colIncrement(_colIncrement), eventProb(_eventProb), eventLength(_eventLength), groupSize(_groupSize) {}
};


class PatternState {
public:
  PatternState(const PatternSettings& settings) : m_settings(settings), m_pos(0), m_currColorIndex(0),
    m_currSpeed(settings.initSpeed), m_currAcceleration(settings.acceleration), m_iter(random16()) {}
  
  PatternSettings& settings() { return m_settings; }
  pos_t& pos() { return m_pos; }
  speed_t& currSpeed() { return m_currSpeed; }      
  colind_t& currColorIndex() { return m_currColorIndex; }
  uint32_t iter() { return m_iter; }
  
  void update();
  
private:
  PatternSettings m_settings;
  pos_t m_pos;
  colind_t m_currColorIndex;
  speed_t m_currSpeed;
  accel_t m_currAcceleration;
  uint32_t m_iter;
};


class Pattern {
public:
  Pattern(CRGB* leds, Palette palette, PatternSettings settings) 
    : m_leds(leds), m_palette(palette), m_state(settings) {}
    
  void setPalette(Palette palette) {
    m_palette = palette;
  }

  PatternState& state() { return m_state; }

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
  void sparkle();

  // simulate fireworks
  void fireworks();

  // all colors the same, changing together
  void consistent();
    
  Palette& palette() { return m_palette; }

private:
  void gradient(bool isWave, bool singleWave);

  CRGB* m_leds;
  Palette m_palette;
  PatternState m_state;
};

#endif

