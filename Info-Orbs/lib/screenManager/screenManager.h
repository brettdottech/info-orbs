#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// Include any necessary libraries here

#include <TFT_eSPI.h>
#include <SPI.h>

#define NUM_SCREENS 5

#define SCREEN_1_CS 15
#define SCREEN_2_CS 2
#define SCREEN_3_CS 27
#define SCREEN_4_CS 4
#define SCREEN_5_CS 5

#define DISPLAY_ROTATION 0

// Define your class or functions here

class ScreenManager {
public:
    ScreenManager(TFT_eSPI& tft);

    TFT_eSPI& getDisplay();

    void selectScreen(int screen);
    void selectAllScreens();
    void reset();

    void fillAllScreens(uint32_t color);

private:
    uint8_t screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI& tft;
};

#endif // SCREENMANAGER_H