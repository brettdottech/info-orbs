#include "screenManager.h"
#include <Arduino.h>

ScreenManager::ScreenManager(TFT_eSPI &tft) : m_tft(tft) {

  for (int i = 0; i < NUM_SCREENS; i++) {
    pinMode(m_screen_cs[i], OUTPUT);
    digitalWrite(m_screen_cs[i], LOW);
  }

  m_tft.init();
  m_tft.setRotation(DISPLAY_ROTATION);
  m_tft.fillScreen(TFT_WHITE);
  m_tft.setTextDatum(MC_DATUM);
  reset();

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

TFT_eSPI &ScreenManager::getDisplay() {
  return m_tft;
}

// Selects a single screen
void ScreenManager::selectScreen(int screen) {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(m_screen_cs[i], i == screen ? LOW : HIGH);
  }
}

// Fills all screens with a color
void ScreenManager::fillAllScreens(uint32_t color) {
  selectAllScreens();
  m_tft.fillScreen(color);
  reset();
}

// clears all screens by resetting them to black
void ScreenManager::clearAllScreens() {
  fillAllScreens(TFT_BLACK);
}

// clears one screens by resetting it to black
void ScreenManager::clearScreen(int screen) {
  selectScreen(screen);
  m_tft.fillScreen(TFT_BLACK);
}

// Selects all screens
// I don't think that state should be used, It's kinda wierd saying "ow select
// all the screens to "off"
void ScreenManager::selectAllScreens() {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(m_screen_cs[i], LOW);
  }
}

void ScreenManager::reset() {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(m_screen_cs[i], HIGH);
  }
}
