#include "GlobalResources.h"
#include "MainHelper.h"
#include "clockwidget/ClockWidget.h"
#include "nhlwidget/NHLWidget.h"
#include "stockwidget/StockWidget.h"
#include "weatherwidget/WeatherWidget.h"
#include "wifiwidget/WifiWidget.h"
#include <ArduinoLog.h>

TFT_eSPI tft = TFT_eSPI();

GlobalTime *globalTime{nullptr};
WifiWidget *wifiWidget{nullptr};
ScreenManager *sm{nullptr};
ConfigManager *config{nullptr};
OrbsWiFiManager *wifiManager{nullptr};
WidgetSet *widgetSet{nullptr};

void addWidgets() {
    // Always add clock
    widgetSet->add(new ClockWidget(*sm, *config));

#if INCLUDE_WEATHER != WIDGET_DISABLED
    widgetSet->add(new WeatherWidget(*sm, *config));
#endif

#if INCLUDE_STOCK != WIDGET_DISABLED
    widgetSet->add(new StockWidget(*sm, *config));
#endif

#if INCLUDE_NHL != WIDGET_DISABLED
    widgetSet->add(new NHLWidget(*sm, *config));
#endif
}

void setup() {
    // Initialize global resources
    initializeGlobalResources();

#ifdef SERIAL_INTERFACE_INIT_DELAY
    // Add a delay to allow the serial interface to initialize
    delay(SERIAL_INTERFACE_INIT_DELAY);
#endif

    Serial.begin(115200);

    // Clear the serial buffer of any garbage
    while (Serial.available() > 0) {
        Serial.read();
    }

#ifdef LOG_TIMESTAMP
    Log.setPrefix(MainHelper::printPrefix);
#endif
    Log.begin(LOG_LEVEL, &Serial);
    Log.noticeln("🚀 Starting up...");
    Log.noticeln("PCB Version: %s", PCB_VERSION);

    wifiManager = new OrbsWiFiManager();
    config = new ConfigManager(*wifiManager);
    sm = new ScreenManager(tft);
    widgetSet = new WidgetSet(sm);

    // Pass references to MainHelper
    MainHelper::init(wifiManager, config, sm, widgetSet);
    MainHelper::setupLittleFS();
    MainHelper::setupConfig();
    MainHelper::setupButtons();
    MainHelper::showWelcome();

    pinMode(BUSY_PIN, OUTPUT);
    Log.noticeln("Connecting to WiFi");
    wifiWidget = new WifiWidget(*sm, *config, *wifiManager);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();
    addWidgets();
    config->setupWebPortal();
    MainHelper::resetCycleTimer();
}

void loop() {
    // MainHelper::watchdogReset();
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
        MainHelper::updateBrightnessByTime(globalTime->getHour24());
        widgetSet->drawCurrent();

        MainHelper::checkCycleWidgets();
        wifiManager->process();
        TaskManager::getInstance()->processAwaitingTasks();
        TaskManager::getInstance()->processTaskResponses();
    }
#ifdef MEMORY_DEBUG_INTERVAL
    ShowMemoryUsage::printSerial();
#endif
    MainHelper::restartIfNecessary();
}
