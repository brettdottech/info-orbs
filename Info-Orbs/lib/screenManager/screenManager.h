#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// Include any necessary libraries here
#include <config.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#define NUM_SCREENS 5

// Define your class or functions here

class ScreenManager {
public:
    ScreenManager(TFT_eSPI& tft);

    TFT_eSPI& getDisplay();

    void selectScreen(int screen);
    void selectAllScreens();
    void reset();

    void fillAllScreens(uint32_t color);
    void clearAllScreens();

    void clearScreen(int screen);

private:
    uint8_t m_screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI& m_tft;
};

#endif // SCREENMANAGER_H