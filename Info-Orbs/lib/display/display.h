#ifndef DISPLAY_H
#define DISPLAY_H

// Include any necessary libraries here

#include <TFT_eSPI.h>
#include <SPI.h>

#define NUM_SCREENS 5

#define SCREEN_1_CS 9
#define SCREEN_2_CS 10
#define SCREEN_3_CS 21
#define SCREEN_4_CS 18
#define SCREEN_5_CS 23

#define DISPLAY_ROTATION 2

// Define your class or functions here

class Display
{
public:
    Display(TFT_eSPI& tft);

    void displayText(String text);
    void displayDidget(int screen, String text, int font, int size, uint32_t fontColor);

private:
    uint8_t screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI& tft;

    void selectScreen(int screen, bool state);

    void selectAllScreens(bool state);
};

#endif // DISPLAY_H