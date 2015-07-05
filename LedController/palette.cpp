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
uint16_t interp16(uint16_t start, uint16_t end, uint16_t index, uint16_t length) {
  if (index >= length) return end;
  return start + index*((int32_t)end-start)/length;
}

CRGB getGradientColor(palind_t palIndex, colind_t gradientIndex) {
  gradientIndex = gradientIndex % PALETTE_SIZE;    

  uint8_t colIndex1 = gradientIndex / GRADIENT_SIZE;
  uint8_t colIndex2 = (colIndex1 + 1) % PALETTE_SIZE_BLOCKS;
  
  CRGB col1 = colorLookup(palIndex * PALETTE_SIZE_BLOCKS + colIndex1);
  CRGB col2 = colorLookup(palIndex * PALETTE_SIZE_BLOCKS + colIndex2);

  // p("gradInd="); p(gradientIndex); p(" subGradInd="); p(subGradientIndex); p(" colInd1/2="); p(colIndex1); p("/"); p(colIndex2); p("\n");
  // p("gradSize="); p(gradientSize); p(" subGradSize="); p(subGradientSize); p("\n");
  // p("rgb1="); p(r1); p(" "); p(g1); p(" "); p(b1); p(" ");
  // p("rgb2="); p(r2); p(" "); p(g2); p(" "); p(b2); p("\n");

#if 0
  uint8_t f2 = gradientIndex % GRADIENT_SIZE;
  uint8_t f1 = 256 - f2;

  uint8_t r1 = scale8_LEAVING_R1_DIRTY(col1.r, f1);
  uint8_t g1 = scale8_LEAVING_R1_DIRTY(col1.g, f1);
  uint8_t b1 = scale8_LEAVING_R1_DIRTY(col1.b, f1);
  uint8_t r2 = scale8_LEAVING_R1_DIRTY(col2.r, f2);
  uint8_t g2 = scale8_LEAVING_R1_DIRTY(col2.g, f2);
  uint8_t b2 = scale8_LEAVING_R1_DIRTY(col2.b, f2);
  cleanup_R1();

  return CRGB(r1+r2, g1+g2, b1+b2);
#else
  uint8_t subGradientIndex = gradientIndex % GRADIENT_SIZE;  
  col_t r = interp16(col1.r, col2.r, subGradientIndex, GRADIENT_SIZE);
  col_t g = interp16(col1.g, col2.g, subGradientIndex, GRADIENT_SIZE);
  col_t b = interp16(col1.b, col2.b, subGradientIndex, GRADIENT_SIZE);

  return CRGB(r, g, b);
#endif

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

// #if USE_FASTLED_PALETTES == 1
//   colIndex %= PALETTE_SIZE;
//   colind_t colIndex2 = colIndex % (PALETTE_SIZE / NUM_COLORS_PER_PALETTE);
//   uint8_t rgbPaletteInd = colIndex / (PALETTE_SIZE / NUM_COLORS_PER_PALETTE);
//   p("colIndex2="); p(colIndex2); p(" rgbPalInd="); p(rgbPaletteInd); p("\n");

//   CRGB rgb = ColorFromPalette(m_rgbPalette[rgbPaletteInd], colIndex2);
//   ditherCycle++;
//   rgb = CRGB(gamma(rgb.r, ditherCycle), gamma(rgb.g, ditherCycle), gamma(rgb.b, ditherCycle));

// #else
//  return rgb;
// #endif

  CRGB rgb = getGradientColor(m_palIndex, colIndex);
  rgb = CRGB(gamma(rgb.r, ditherCycle), gamma(rgb.g, ditherCycle), gamma(rgb.b, ditherCycle));

#if MIN_SATURATION > 0
  CHSV hsv = rgb2hsv_approximate(rgb);
  hsv.s = max(hsv.s, MIN_SATURATION);
  hsv2rgb_rainbow(hsv, rgb);
#endif

  return rgb;
//#endif
}
