#include "screenManager.h"
#include <Arduino.h>

ScreenManager::ScreenManager(TFT_eSPI &tft) : tft(tft) {

  for (int i = 0; i < NUM_SCREENS; i++) {
    pinMode(screen_cs[i], OUTPUT);
    digitalWrite(screen_cs[i], LOW);
  }

  tft.init();
  tft.setRotation(DISPLAY_ROTATION);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  reset();
}

// Selects a single screen
void ScreenManager::selectScreen(int screen) {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(screen_cs[i], i == screen ? LOW : HIGH);
  }
}

// Selects all screens
// I don't think that state should be used, It's kinda wierd saying "ow select
// all the screens to "off"
void ScreenManager::selectAllScreens() {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(screen_cs[i], LOW);
  }
}

void ScreenManager::reset() {
  for (int i = 0; i < NUM_SCREENS; i++) {
    digitalWrite(screen_cs[i], HIGH);
  }
}
