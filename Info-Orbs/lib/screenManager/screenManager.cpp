#include <Arduino.h>
#include "screenManager.h"

ScreenManager::ScreenManager(TFT_eSPI &tft) : tft(tft)
{
    for (int i = 0; i < NUM_SCREENS; i++)
    {
        pinMode(screen_cs[i], OUTPUT);
        digitalWrite(screen_cs[i], LOW);
    }

    tft.init();
    tft.setRotation(DISPLAY_ROTATION);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    selectAllScreens(false);
}

// Selects a single screen
void ScreenManager::selectScreen(int screen, bool state)
{
    for (int i = 0; i < NUM_SCREENS; i++)
    {
        if (i == screen)
        {
            digitalWrite(screen_cs[screen], state ? LOW : HIGH);
        } else {
            digitalWrite(screen_cs[i], state ? HIGH : LOW);
        }
    }
    
}

// Selects all screens
void ScreenManager::selectAllScreens(bool state)
{
    for (int i = 0; i < NUM_SCREENS; i++)
    {
        digitalWrite(screen_cs[i], state ? LOW : HIGH);
    }
}