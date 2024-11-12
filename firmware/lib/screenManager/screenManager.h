#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// Include any necessary libraries here
#include <config_helper.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <OpenFontRender.h>
#include "../../include/ttffonts.h"

#define NUM_SCREENS 5

// Define your class or functions here

class ScreenManager {
public:
    ScreenManager(TFT_eSPI& tft);

    TFT_eSPI& getDisplay();
    OpenFontRender &getRender();

    // Screen& getScreen(int screen);

    void selectScreen(int screen);
    void selectAllScreens();
    void reset();

    void fillAllScreens(uint32_t color);
    void clearAllScreens();
    void setFontColor(uint32_t color);
    void setFontColor(uint32_t color, uint32_t background);
    void setBackgroundColor(uint32_t color);
    void setFontSize(uint32_t size);
    void setAlignment(Align align);

    void fillScreen(uint32_t color);
    void clearScreen(int screen);

    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align, u_int16_t fgColor, uint16_t bgColor);
    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align, u_int16_t fgColor);
    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align);
    void drawString(const char *text, int x, int y);

    void drawCentreString(const char *text, int x, int y, unsigned int fontSize);

    // Drawing stuff
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color);
    void drawArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool smoothArc = true);

private:
    uint8_t m_screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI& m_tft;
    OpenFontRender m_render;
    // Screen *m_screens[NUM_SCREENS];


};

#endif // SCREENMANAGER_H