#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#include "palette.h"
#include "gamma.h"

#define USE_FASTLED_PALETTES 0

uint32_t colorLookup(int index) {
  return pgm_read_dword(colorTable + index);
}

#if USE_FASTLED_PALETTES == 0
void colorToRgb(uint32_t col, uint8_t& r, uint8_t& g, uint8_t& b) {
  r = (col >> 16) & 255;
  g = (col >> 8) & 255;
  b = col & 255;
}

uint16_t interp16(uint16_t start, uint16_t end, uint16_t index, uint16_t length) {
  if (index >= length) return end;
  return start + index*((int32_t)end-start)/length;
}

void getGradientColor(int palIndex, uint32_t gradientIndex, int gradientSize, uint8_t& r, uint8_t& g, uint8_t& b) {
  gradientIndex = (gradientIndex + gradientSize) % gradientSize;    

  int subGradientSize = gradientSize / NUM_COLORS_PER_PALETTE;

  int colIndex1 = gradientIndex / subGradientSize;
  int colIndex2 = (colIndex1 + 1) % NUM_COLORS_PER_PALETTE;
  
  uint8_t r1, g1, b1, r2, g2, b2;
  colorToRgb(colorLookup(palIndex * NUM_COLORS_PER_PALETTE + colIndex1), r1, g1, b1);
  colorToRgb(colorLookup(palIndex * NUM_COLORS_PER_PALETTE + colIndex2), r2, g2, b2);

  int subGradientIndex = gradientIndex % subGradientSize;

  p("gradInd="); p(gradientIndex); p(" subGradInd="); p(subGradientIndex); p(" colInd1/2="); p(colIndex1); p("/"); p(colIndex2); p("\n");
  p("gradSize="); p(gradientSize); p(" subGradSize="); p(subGradientSize); p("\n");
  p("rgb1="); p(r1); p(" "); p(g1); p(" "); p(b1); p(" ");
  p("rgb2="); p(r2); p(" "); p(g2); p(" "); p(b2); p("\n");

  r = interp16(r1, r2, subGradientIndex, subGradientSize);
  g = interp16(g1, g2, subGradientIndex, subGradientSize);
  b = interp16(b1, b2, subGradientIndex, subGradientSize);
  p("rgb="); p(r); p(" "); p(g); p(" "); p(b); p("\n");
}
#endif


Palette::Palette(int palIndex, int palSize)
    : m_palIndex(palIndex), m_palSize(palSize) {
#if USE_FASTLED_PALETTES == 1
  for (uint8_t i = 0; i < NUM_COLORS_PER_PALETTE; i++) {
    CRGB rgb0(colorLookup(palIndex * NUM_COLORS_PER_PALETTE + i));
    CRGB rgb1(colorLookup(palIndex * NUM_COLORS_PER_PALETTE + 
      (i + 1) % NUM_COLORS_PER_PALETTE));

    m_rgbPalette[i] = CRGBPalette16(rgb0, rgb1);
  }
#endif
}

CRGB Palette::getColor(colind_t colIndex) {
  static uint16_t ditherCycle = 0;

#if USE_FASTLED_PALETTES == 1
  colIndex %= PALETTE_SIZE;
  colind_t colIndex2 = colIndex % (PALETTE_SIZE / NUM_COLORS_PER_PALETTE);
  uint8_t rgbPaletteInd = colIndex / (PALETTE_SIZE / NUM_COLORS_PER_PALETTE);
  p("colIndex2="); p(colIndex2); p(" rgbPalInd="); p(rgbPaletteInd); p("\n");

  CRGB rgb = ColorFromPalette(m_rgbPalette[rgbPaletteInd], colIndex2);
  ditherCycle++;
  return CRGB(gamma(rgb.r, ditherCycle), gamma(rgb.g, ditherCycle), gamma(rgb.b, ditherCycle));
#else
  uint8_t r, g, b;
  getGradientColor(m_palIndex, colIndex, m_palSize, r, g, b);
  CRGB col = CRGB(gamma(r, ditherCycle), gamma(g, ditherCycle), gamma(b, ditherCycle));
  return col;
#endif
}
