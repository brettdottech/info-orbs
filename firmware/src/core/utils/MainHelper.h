#ifndef MAINHELPER_H
#define MAINHELPER_H

#include "Button.h"
#include "ConfigManager.h"
#include "OrbsWiFiManager.h"
#include "ScreenManager.h"
#include "TFT_eSPI.h"
#include "WidgetSet.h"
#include "icons.h"
#include <Arduino.h>

class MainHelper {
public:
    static void init(WiFiManager *wm, ConfigManager *cm, WidgetSet *ws);
    static void isrButtonChangeLeft();
    static void isrButtonChangeMiddle();
    static void isrButtonChangeRight();

    static void setupButtons();
    static void setupConfig();

    static void buttonPressed(uint8_t buttonId, ButtonState state);
    static void checkButtons();
    static void checkCycleWidgets();

    static void setupWebPortalEndpoints();
    static void handleEndpointButton();
    static void handleEndpointButtons();

    static void showWelcome(ScreenManager *screenManager);
    static void resetCycleTimer();

    static void showMemoryUsage(bool force = false);

private:
    static WiFiManager *s_wifiManager;
    static ConfigManager *s_configManager;
    static WidgetSet *s_widgetSet;

    static int s_widgetCycleDelay;
    static unsigned long s_widgetCycleDelayPrev;
    static bool s_invertedOrbs;
    static unsigned long s_lastMemoryInfo;
};

#endif