#ifndef RENDERER_H
#define RENDERER_H

#include "ttf-fonts.h"
#include "screenmanager\ScreenManager.h"
#include "Settings.h"

struct Renderer : private ScreenManager {
private:
  Settings* _Settings;
public:
  Renderer(Settings& settings, TFT_eSPI &tft) : ScreenManager(tft) { _Settings = &settings; }
  void __selectScreen(int screen) { selectScreen(screen); }
  void clearScreen() { fillScreen(_Settings->getBackgroundColor()); }
  void selectFont(TTF_Font font) { setFont(font); }
  void text(uint16_t x, uint16_t y, uint16_t fontSize, Align alignment, const String& text)
  {
    drawString(text, x, y, fontSize, alignment, _Settings->getForegroundColor(), _Settings->getBackgroundColor(), true);
  }
  inline const uint16_t getCenterX() { return 120; }
  inline const uint16_t getCenterY() { return 120; }
};

#endif