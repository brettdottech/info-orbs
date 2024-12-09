#include "Button.h"
#include "ConfigManager.h"
#include "GlobalTime.h"
#include "ScreenManager.h"
#include "Utils.h"
#include "WidgetSet.h"
#include "clockwidget/ClockWidget.h"
#include "config_helper.h"
#include "icons.h"
#include "weatherwidget/WeatherWidget.h"
#include "webdatawidget/WebDataWidget.h"
#include "wifiwidget/WifiWidget.h"
#include <Arduino.h>

#ifdef STOCK_TICKER_LIST
    #include "stockwidget/StockWidget.h"
#endif
#ifdef PARQET_PORTFOLIO_ID
    #include "parqetwidget/ParqetWidget.h"
#endif
#ifdef MQTT_WIDGET_HOST
    #include "mqttwidget/MQTTWidget.h"
#endif

TFT_eSPI tft = TFT_eSPI();

#ifdef WIDGET_CYCLE_DELAY
int m_widgetCycleDelay = WIDGET_CYCLE_DELAY; // Automatically cycle widgets every X seconds, set to 0 to disable
#else
int m_widgetCycleDelay = 0;
#endif
unsigned long m_widgetCycleDelayPrev = 0;
bool m_invertedOrbs = INVERTED_ORBS;

Button buttonLeft(BUTTON_LEFT);
Button buttonOK(BUTTON_OK);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{nullptr};

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

ScreenManager *sm;
ConfigManager *config;
WiFiManager *wifiManager;
WidgetSet *widgetSet;

/**
 * The ISR handlers must be static
 */
void isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void isrButtonChangeMiddle() { buttonOK.isrButtonChange(); }
void isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void setupButtons() {
    buttonLeft.begin();
    buttonOK.begin();
    buttonRight.begin();

    attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), isrButtonChangeLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_OK), isrButtonChangeMiddle, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), isrButtonChangeRight, CHANGE);
}

void setupConfig() {
    config->addConfigInt("General", "widgetCycDelay", &m_widgetCycleDelay, "Automatically cycle widgets every X seconds, set to 0 to disable");
    config->addConfigBool("General", "invertedOrbs", &m_invertedOrbs, "Inverted Orbs (enable if using InfoOrbs upside down)");
    tft.setRotation(m_invertedOrbs ? 2 : 0);
}

// Forward declaration
void buttonPressed(uint8_t buttonId, ButtonState state);

// Handle simulated button state
void handleEndpointButton() {
    if (wifiManager->server->hasArg("name") && wifiManager->server->hasArg("state")) {
        String inButton = wifiManager->server->arg("name");
        String inState = wifiManager->server->arg("state");
        uint8_t buttonId = Button::stringToButtonId(inButton);
        ButtonState state = Button::stringToButtonState(inState);

        if (buttonId != 0 && state != BTN_NOTHING) {
            buttonPressed(buttonId, state);
            wifiManager->server->send(200, "text/plain", "OK " + inButton + "/" + inState + " -> " + String(buttonId) + "/" + String(state));
            return;
        }
    }
    wifiManager->server->send(500, "text/plain", "ERR");
}

// Show button web page
void handleEndpointButtons() {
    String msg = "<html><body><table>";
    String buttons[] = {"left", "middle", "right"};
    String states[] = {"short", "medium", "long"};
    int numButtons = 3; // number of buttons
    int numStates = 3; // number of states
    for (int s = 0; s < numStates; s++) {
        msg += "<tr>";
        for (int b = 0; b < numButtons; b++) {
            msg += "<td style='padding: 10px;'><button style='height: 50px; width: 150px;' onclick=\"sendReq('" + buttons[b] + "', '" + states[s] + "')\">" + buttons[b] + " " + states[s] + "</button></td>";
        }
        msg += "</tr>";
    }
    msg += "</table><script>function sendReq(name, state) {fetch(`/button?name=${name}&state=${state}`);}</script></body></html>";
    wifiManager->server->send(200, "text/html", msg);
}

void setupWebPortalEndpoints() {
    // To simulate button presses call e.g. http://<ip>/button?name=right&state=short
    wifiManager->server->on("/button", handleEndpointButton);
    wifiManager->server->on("/buttons", handleEndpointButtons);
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    wifiManager = new WiFiManager();
    config = new ConfigManager(*wifiManager);

    setupConfig();
    setupButtons();

    sm = new ScreenManager(tft);
    sm->fillAllScreens(TFT_BLACK);
    sm->setFontColor(TFT_WHITE);

    sm->selectScreen(0);
    sm->drawCentreString("Welcome", ScreenCenterX, ScreenCenterY, 29);

    sm->selectScreen(1);
    sm->drawCentreString("Info Orbs", ScreenCenterX, ScreenCenterY - 50, 22);
    sm->drawCentreString("by", ScreenCenterX, ScreenCenterY - 5, 22);
    sm->drawCentreString("brett.tech", ScreenCenterX, ScreenCenterY + 30, 22);
    sm->setFontColor(TFT_RED);
    sm->drawCentreString("version: 1.1.0", ScreenCenterX, ScreenCenterY + 65, 14);

    sm->selectScreen(2);

    sm->drawJpg(0, 0, logo_start, logo_end - logo_start);

    widgetSet = new WidgetSet(sm, *config);

#ifdef GC9A01_DRIVER
    Serial.println("GC9A01 Driver");
#endif
#if HARDWARE == WOKWI
    Serial.println("Wokwi Build");
#endif

    pinMode(BUSY_PIN, OUTPUT);
    Serial.println("Connecting to WiFi");

    wifiWidget = new WifiWidget(*sm, *config, *wifiManager);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();

    widgetSet->add(new ClockWidget(*sm, *config));
#ifdef PARQET_PORTFOLIO_ID
    widgetSet->add(new ParqetWidget(*sm, *config));
#endif
#ifdef STOCK_TICKER_LIST
    widgetSet->add(new StockWidget(*sm, *config));
#endif
    widgetSet->add(new WeatherWidget(*sm, *config));
#ifdef WEB_DATA_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, *config, WEB_DATA_WIDGET_URL));
#endif
#ifdef WEB_DATA_STOCK_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, *config, WEB_DATA_STOCK_WIDGET_URL));
#endif
#ifdef MQTT_WIDGET_HOST
    widgetSet->add(new MQTTWidget(*sm, *config, MQTT_WIDGET_HOST, MQTT_WIDGET_PORT));
#endif

    config->setupWiFiManager();

    m_widgetCycleDelayPrev = millis();
}

void checkCycleWidgets() {
    if (m_widgetCycleDelay > 0 && (m_widgetCycleDelayPrev == 0 || (millis() - m_widgetCycleDelayPrev) >= m_widgetCycleDelay * 1000)) {
        widgetSet->next();
        m_widgetCycleDelayPrev = millis();
    }
}

void buttonPressed(uint8_t buttonId, ButtonState state) {
    if (state == BTN_NOTHING) {
        // nothing to do
        return;
    }
    // Reset cycle timer whenever a button is pressed
    if (buttonId == BUTTON_LEFT && state == BTN_SHORT) {
        // Left short press cycles widgets backward
        Serial.println("Left button short pressed -> switch to prev Widget");
        m_widgetCycleDelayPrev = millis();
        widgetSet->prev();
    } else if (buttonId == BUTTON_RIGHT && state == BTN_SHORT) {
        // Right short press cycles widgets forward
        Serial.println("Right button short pressed -> switch to next Widget");
        m_widgetCycleDelayPrev = millis();
        widgetSet->next();
    } else {
        // Everying else that is not BTN_NOTHING will be forwarded to the current widget
        if (buttonId == BUTTON_LEFT) {
            Serial.printf("Left button pressed, state=%d\n", state);
            m_widgetCycleDelayPrev = millis();
            widgetSet->buttonPressed(BUTTON_LEFT, state);
        } else if (buttonId == BUTTON_OK) {
            Serial.printf("Middle button pressed, state=%d\n", state);
            m_widgetCycleDelayPrev = millis();
            widgetSet->buttonPressed(BUTTON_OK, state);
        } else if (buttonId == BUTTON_RIGHT) {
            Serial.printf("Right button pressed, state=%d\n", state);
            m_widgetCycleDelayPrev = millis();
            widgetSet->buttonPressed(BUTTON_RIGHT, state);
        }
    }
}

void checkButtons() {
    ButtonState leftState = buttonLeft.getState();
    if (leftState != BTN_NOTHING) {
        buttonPressed(BUTTON_LEFT, leftState);
    }
    ButtonState middleState = buttonOK.getState();
    if (middleState != BTN_NOTHING) {
        buttonPressed(BUTTON_OK, middleState);
    }
    ButtonState rightState = buttonRight.getState();
    if (rightState != BTN_NOTHING) {
        buttonPressed(BUTTON_RIGHT, rightState);
    }
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
            setupWebPortalEndpoints();
        }
        globalTime->updateTime();

        checkButtons();

        widgetSet->updateCurrent();
        widgetSet->updateBrightnessByTime(globalTime->getHour24());
        widgetSet->drawCurrent();

        checkCycleWidgets();

        wifiWidget->processWebPortalRequests();
    }
}
