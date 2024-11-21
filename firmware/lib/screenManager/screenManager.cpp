#include "screenManager.h"
#include <Arduino.h>

ScreenManager::ScreenManager(TFT_eSPI &tft) : m_tft(tft) {

  for (int i = 0; i < NUM_SCREENS; i++) {
    pinMode(m_screen_cs[i], OUTPUT);
    digitalWrite(m_screen_cs[i], LOW);
  }

  m_tft.init();
  m_tft.setRotation(INVERTED_ORBS ? 2 : 0);
  m_tft.fillScreen(TFT_WHITE);
  m_tft.setTextDatum(MC_DATUM);
  reset();

  // I'm not sure which cache size is actually good.
  // Needs testing.
  m_render.setCacheSize(128, 128, 8192);
  setFont(DEFAULT_FONT);
  m_render.setDrawer(m_tft);

  Serial.println("ScreenManager initialized");
  Serial.println("TFT_MOSI:" + String(TFT_MOSI));
  Serial.println("TFT_MISO:" + String(TFT_MISO));
  Serial.println("TFT_SCLK:" + String(TFT_SCLK));
  Serial.println("TFT_CS:" + String(TFT_CS));
  Serial.println("TFT_DC:" + String(TFT_DC));
  Serial.println("TFT_RST:" + String(TFT_RST));
  Serial.println("SCREEN_1_CS:" + String(SCREEN_1_CS));
  Serial.println("SCREEN_2_CS:" + String(SCREEN_2_CS));
  Serial.println("SCREEN_3_CS:" + String(SCREEN_3_CS));
  Serial.println("SCREEN_4_CS:" + String(SCREEN_4_CS));
  Serial.println("SCREEN_5_CS:" + String(SCREEN_5_CS));

}

void ScreenManager::setFont(TTF_Font font) {
  if (font == m_curFont) {
    // nothing to do
    return;
  }
  m_render.unloadFont();
  // Font is now unloaded
  m_curFont = TTF_Font::NONE;
  if (font == TTF_Font::NONE) {
    // just unload
    return;
  }
  // 0 is success
  FT_Error error = 1;
  switch (font)
  {
  case ROBOTO_REGULAR:
    error = m_render.loadFont(robotoRegular_start, robotoRegular_end-robotoRegular_start);
    break;
  
  case FINAL_FRONTIER:
    error = m_render.loadFont(finalFrontier_start, finalFrontier_end-finalFrontier_start);
    break;

  case DSEG7:
    error = m_render.loadFont(dseg7_start, dseg7_end-dseg7_start);
    break;

  case DSEG14:
    error = m_render.loadFont(dseg14_start, dseg14_end-dseg14_start);
    break;
  }
  if (error == 0) {
    m_curFont = font;
  } else {
    Serial.printf("Unable to load TTF font %d\n", font);
  } 
}

TFT_eSPI &ScreenManager::getDisplay() {
  return m_tft;
}

OpenFontRender &ScreenManager::getRender() {
  return m_render;
}

// Selects a single screen
void ScreenManager::selectScreen(int screen) {
  for (int i = 0; i < NUM_SCREENS; i++) {
    int currentDisplay = INVERTED_ORBS ? NUM_SCREENS - i - 1 : i;
    digitalWrite(m_screen_cs[currentDisplay], i == screen ? LOW : HIGH);
  }
}

// Fills all screens with a color
void ScreenManager::fillAllScreens(uint32_t color) {
  selectAllScreens();
  m_tft.fillScreen(color);
  reset();
}

// Clears all screens by resetting them to black
void ScreenManager::clearAllScreens() {
  fillAllScreens(TFT_BLACK);
}

// Clears one screens by resetting it to black
void ScreenManager::clearScreen(int screen) {
  if (screen >= 0) {
    selectScreen(screen);
  }
  fillScreen(TFT_BLACK);
}

void ScreenManager::fillScreen(uint32_t color) {
  m_tft.fillScreen(color);
  // Set background for aliasing as well
  m_render.setBackgroundColor(color);
}

void ScreenManager::setFontColor(uint32_t color) {
  m_render.setFontColor(color);
}

void ScreenManager::setFontColor(uint32_t color, uint32_t background) {
  m_render.setFontColor(color);
  m_render.setBackgroundColor(background);
}

void ScreenManager::setBackgroundColor(uint32_t color) {
  m_render.setBackgroundColor(color);
}

void ScreenManager::setAlignment(Align align) {
  m_render.setAlignment(align);
}

void ScreenManager::setFontSize(uint32_t size) {
  m_render.setFontSize(size);
}

// Selects all screens
// I don't think that state should be used, It's kinda weird saying "ow select
// all the screens to "off"
void ScreenManager::selectAllScreens() {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(m_screen_cs[i], LOW);
  }
}

// Unselect all screens
void ScreenManager::reset() {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(m_screen_cs[i], HIGH);
  }
}

unsigned int ScreenManager::calculateFitFontSize(uint32_t limit_width, uint32_t limit_height, Layout layout, const String &text) {
  unsigned int calcFontSize = m_render.calculateFitFontSize(limit_width, limit_height, layout, text.c_str());
  // Serial.printf("calcFitFontSize: t=%s, w=%d, h=%d -> fs=%d\n", str, limit_width, limit_height, calcFontSize);
  return calcFontSize;
}

void ScreenManager::drawString(const String &text, int x, int y) {
  // Use current font size and alignment
  drawString(text, x, y, 0, m_render.getAlignment());
}

void ScreenManager::drawString(const String &text, int x, int y, unsigned int fontSize, Align align) {
  drawString(text, x, y, fontSize, align, m_render.getFontColor());
}

void ScreenManager::drawString(const String &text, int x, int y, unsigned int fontSize, Align align, u_int16_t fgColor) {
  uint16_t bgColor = m_render.getBackgroundColor();
  drawString(text, x, y, fontSize, align, fgColor, m_render.getBackgroundColor());
 }

void ScreenManager::drawString(const String &text, int x, int y, unsigned int fontSize, Align align, u_int16_t fgColor, uint16_t bgColor, bool applyScale) {
  if (fontSize == 0) {
    // Keep current font size
    fontSize == m_render.getFontSize();
  } else if (applyScale) {
    fontSize = getScaledFontSize(fontSize);
  }
  // Dirty hack to correct misaligned Y
  // See https://github.com/takkaO/OpenFontRender/issues/38
  FT_BBox box = m_render.calculateBoundingBox(0,0,fontSize,Align::TopLeft,Layout::Horizontal, text.c_str());
  m_render.setAlignment(align);
  m_render.setFontSize(fontSize);
  m_render.drawString(text.c_str(), x, y - box.yMin, fgColor, bgColor);
}

void ScreenManager::drawCentreString(const String &text, int x, int y, unsigned int fontSize) {
  drawString(text, x, y, fontSize, Align::MiddleCenter);
}

void ScreenManager::drawFittedString(const String &text, int x, int y, int limit_w, int limit_h, Align align) {
  unsigned int fontSize = calculateFitFontSize(limit_w, limit_h, Layout::Horizontal, text);
  drawString(text, x, y, fontSize, align, m_render.getFontColor(), m_render.getBackgroundColor(), false);
}

void ScreenManager::drawFittedString(const String &text, int x, int y, int limit_w, int limit_h) {
  drawFittedString(text, x, y, limit_w, limit_h, m_render.getAlignment());
}

void ScreenManager::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  m_tft.fillRect(x, y, w, h, color);
}

void ScreenManager::drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color) {
  m_tft.drawLine(xs, ys, xe, ye, color);
}

void ScreenManager::drawArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool smoothArc) {
  m_tft.drawArc(x, y, r, ir, startAngle, endAngle, fg_color, bg_color, smoothArc);
}

void ScreenManager::drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool roundEnds) {
  m_tft.drawSmoothArc(x, y, r, ir, startAngle, endAngle, fg_color, bg_color, roundEnds);
}

void ScreenManager::fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color) {
  m_tft.fillTriangle(x1, y1, x2, y2, x3, y3, color);
}

unsigned int ScreenManager::getScaledFontSize(unsigned int fontSize) {
  for (TTF_FontMetric metric : ttfFontMetrics) {
    if (metric.font == m_curFont) {
      return round(metric.scale * fontSize);
    }
  }
  return fontSize;
}
