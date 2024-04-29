#include <Arduino.h>
#include "display.h"

Display::Display(TFT_eSPI &tft) : tft(tft)
{
    Serial.println("Display Constructor");
    for (int i = 0; i < NUM_SCREENS; i++)
    {
        pinMode(screen_cs[i], OUTPUT);
        digitalWrite(screen_cs[i], LOW);
    }

    tft.init();
    tft.setRotation(DISPLAY_ROTATION);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    displayText("Display Initialized");
    selectAllScreens(false);
}

void Display::displayText(String text)
{
    tft.fillScreen(TFT_BLACK);
    tft.drawString(text, tft.width() / 2, tft.height() / 2);
}

/*void Display::displayDidget(int screen, String text, int font, int size, uint32_t fontColor) {
  selectScreen(screen, true);
  tft.setTextSize(size);
  if (shadowing == true && font == 7) {
    tft.setTextColor(bgcl, TFT_BLACK);
    tft.drawString("8", centre , centre, z);
    tft.setTextColor(c);
  } else {
    tft.setTextColor(c, TFT_BLACK);
  }
  tft.drawString(y, centre, centre, z);
  selectScreen(screen, false);
}*/

// Selects a single screen
void Display::selectScreen(int screen, bool state)
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
void Display::selectAllScreens(bool state)
{
    for (int i = 0; i < NUM_SCREENS; i++)
    {
        digitalWrite(screen_cs[i], state ? LOW : HIGH);
    }
}