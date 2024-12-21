#ifndef MAINHELPER_H
#define MAINHELPER_H

#include "Button.h"
#include "ConfigManager.h"
#include "OrbsWiFiManager.h"
#include "ScreenManager.h"
#include "ShowMemoryUsage.h"
#include "TFT_eSPI.h"
#include "WidgetSet.h"
#include "git_info.h"
#include <Arduino.h>

// Set defaults if not set in config.h
#ifndef TFT_BRIGHTNESS
    #define TFT_BRIGHTNESS 255
#endif

#if defined(DIM_START_HOUR) && defined(DIM_END_HOUR) && defined(DIM_BRIGHTNESS)
    #define DIM_ENABLED true
#else
    #define DIM_ENABLED false
#endif

#ifndef DIM_START_HOUR
    #define DIM_START_HOUR 22
#endif

#ifndef DIM_END_HOUR
    #define DIM_END_HOUR 7
#endif

#ifndef DIM_BRIGHTNESS
    #define DIM_BRIGHTNESS 128
#endif

#ifndef WIDGET_CYCLE_DELAY
    #define WIDGET_CYCLE_DELAY 0
#endif

#ifndef NTP_SERVER
    #define NTP_SERVER "pool.ntp.org"
#endif

class MainHelper {
public:
    static void init(WiFiManager *wm, ConfigManager *cm, ScreenManager *sm, WidgetSet *ws);
    static void isrButtonChangeLeft();
    static void isrButtonChangeMiddle();
    static void isrButtonChangeRight();

    static void setupButtons();
    static void setupConfig();
    static void setupLittleFS();
    static void showWelcome();

    static void buttonPressed(uint8_t buttonId, ButtonState state);
    static void checkButtons();

    static void checkCycleWidgets();
    static void resetCycleTimer();

    static void setupWebPortalEndpoints();
    static void handleEndpointButton();
    static void handleEndpointButtons();
    static void handleEndpointListFiles();
    static void handleEndpointUploadFile();
    static void handleEndpointDeleteFile();
    static void handleEndpointDownloadFile();
    static void handleEndpointDownloadFilesFromURL();

    static void restartIfNecessary();

    static void updateBrightnessByTime(uint8_t hour24);
};

#endif