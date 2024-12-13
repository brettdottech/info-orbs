#ifndef MAINHELPER_H
#define MAINHELPER_H

#include "Button.h"
#include "ConfigManager.h"
#include "OrbsWiFiManager.h"
#include "ScreenManager.h"
#include "ShowMemoryUsage.h"
#include "TFT_eSPI.h"
#include "WidgetSet.h"
#include <Arduino.h>

class MainHelper {
public:
    static void init(WiFiManager *wm, ConfigManager *cm, ScreenManager *sm, WidgetSet *ws);
    static void isrButtonChangeLeft();
    static void isrButtonChangeMiddle();
    static void isrButtonChangeRight();

    static void setupButtons();
    static void setupConfig();
    static void showWelcome();

    static void buttonPressed(uint8_t buttonId, ButtonState state);
    static void checkButtons();

    static void checkCycleWidgets();
    static void resetCycleTimer();

    static void setupWebPortalEndpoints();
    static void handleEndpointButton();
    static void handleEndpointButtons();
    static void restartIfNecessary();

    static void updateBrightnessByTime(uint8_t hour24);

private:
    static WiFiManager *s_wifiManager;
    static ConfigManager *s_configManager;
    static ScreenManager *s_screenManager;
    static WidgetSet *s_widgetSet;

    static Button buttonLeft;
    static Button buttonOK;
    static Button buttonRight;

    static int s_widgetCycleDelay;
    static unsigned long s_widgetCycleDelayPrev;
    static bool s_invertedOrbs;
    static std::string s_timezoneLocation;
    static int s_tftBrightness;
    static bool s_nightMode;
    static int s_dimStartHour;
    static int s_dimEndHour;
    static int s_dimBrightness;
};

#endif