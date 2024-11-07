#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// Include any necessary libraries here
#include <config_helper.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <OpenFontRender.h>
#include "../../include/roboto_regular.h"
// #include "../../include/NotoSans_Bold.h"

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
    void setFontSize(uint32_t size);
    void setAlignment(Align align);

    void fillScreen(uint32_t color);
    void clearScreen(int screen);

    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align);
    void drawString(const char *text, int x, int y);
    void drawCentreString(const char *text, int x, int y, unsigned int fontSize);

private:
    uint8_t m_screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI& m_tft;
    OpenFontRender m_render;
    // Screen *m_screens[NUM_SCREENS];


};

#endif // SCREENMANAGER_H