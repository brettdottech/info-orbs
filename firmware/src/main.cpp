#include "Button.h"
#include "ConfigManager.h"
#include "GlobalTime.h"
#include "MainHelper.h"
#include "ScreenManager.h"
#include "Utils.h"
#include "WidgetSet.h"
#include "clockwidget/ClockWidget.h"
#include "config_helper.h"
#include "icons.h"
#include "mqttwidget/MQTTWidget.h"
#include "parqetwidget/ParqetWidget.h"
#include "stockwidget/StockWidget.h"
#include "weatherwidget/WeatherWidget.h"
#include "webdatawidget/WebDataWidget.h"
#include "wifiwidget/WifiWidget.h"
#include <Arduino.h>

TFT_eSPI tft = TFT_eSPI();

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{nullptr};

ScreenManager *sm{nullptr};
ConfigManager *config{nullptr};
WiFiManager *wifiManager{nullptr};
WidgetSet *widgetSet{nullptr};

void addWidgets() {
    // Always add clock
    widgetSet->add(new ClockWidget(*sm, *config));
#ifdef INCLUDE_PARQET
    widgetSet->add(new ParqetWidget(*sm, *config));
#endif
#ifdef INCLUDE_STOCK
    widgetSet->add(new StockWidget(*sm, *config));
#endif
    widgetSet->add(new WeatherWidget(*sm, *config));
#ifdef INCLUDE_WEBDATA
    #ifdef WEB_DATA_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, *config, WEB_DATA_WIDGET_URL));
    #endif
    #ifdef WEB_DATA_STOCK_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, *config, WEB_DATA_STOCK_WIDGET_URL));
    #endif
#endif
#ifdef INCLUDE_MQTT
    widgetSet->add(new MQTTWidget(*sm, *config));
#endif
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    wifiManager = new WiFiManager();
    config = new ConfigManager(*wifiManager);
    sm = new ScreenManager(tft);
    widgetSet = new WidgetSet(sm, *config);

    // Pass references to MainHelper
    MainHelper::init(wifiManager, config, widgetSet);

    MainHelper::setupConfig();
    MainHelper::setupButtons();

    MainHelper::showWelcome(sm);

    pinMode(BUSY_PIN, OUTPUT);
    Serial.println("Connecting to WiFi");

    wifiWidget = new WifiWidget(*sm, *config, *wifiManager);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();

    addWidgets();

    config->setupWiFiManager();
    MainHelper::resetCycleTimer();
}

void loop() {
    if (wifiWidget->isConnected() == false) {
        wifiWidget->update();
        wifiWidget->draw();
        widgetSet->setClearScreensOnDrawCurrent(); // Clear screen after wifiWidget
        delay(100);
    } else {
        if (!widgetSet->initialUpdateDone()) {
            widgetSet->initializeAllWidgetsData();
            MainHelper::setupWebPortalEndpoints();
        }
        globalTime->updateTime();

        MainHelper::checkButtons();

        widgetSet->updateCurrent();
        widgetSet->updateBrightnessByTime(globalTime->getHour24());
        widgetSet->drawCurrent();

        MainHelper::checkCycleWidgets();

        wifiWidget->processWebPortalRequests();
    }
}
