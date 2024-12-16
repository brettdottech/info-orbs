#include "MainHelper.h"
#include "clockwidget/ClockWidget.h"
#include "mqttwidget/MQTTWidget.h"
#include "parqetwidget/ParqetWidget.h"
#include "stockwidget/StockWidget.h"
#include "weatherwidget/WeatherWidget.h"
#include "webdatawidget/WebDataWidget.h"
#include "wifiwidget/WifiWidget.h"

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
#ifdef INCLUDE_WEATHER
    widgetSet->add(new WeatherWidget(*sm, *config));
#endif
#ifdef INCLUDE_STOCK
    widgetSet->add(new StockWidget(*sm, *config));
#endif
#ifdef INCLUDE_PARQET
    widgetSet->add(new ParqetWidget(*sm, *config));
#endif
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
    Serial.println("Connecting to WiFi");
    wifiWidget = new WifiWidget(*sm, *config, *wifiManager);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();
    addWidgets();
    config->setupWebPortal();
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
        MainHelper::updateBrightnessByTime(globalTime->getHour24());
        widgetSet->drawCurrent();

        MainHelper::checkCycleWidgets();
        wifiManager->process();
    }
#ifdef MEMORY_DEBUG
    ShowMemoryUsage::printSerial();
#endif
    MainHelper::restartIfNecessary();
}
