#include "Button.h"
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
#include <ClickEncoder.h>

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
    unsigned long m_widgetCycleDelay = WIDGET_CYCLE_DELAY * 1000; // Automatically cycle widgets every X seconds, set to 0 to disable
#else
    unsigned long m_widgetCycleDelay = 0;
#endif
unsigned long m_widgetCycleDelayPrev = 0;

#ifdef USE_ROTARY_ENCODER
    unsigned long lastTimeEncoderService = 0;
    unsigned long lastTimeEncoderTurn = 0;
    unsigned long lastTimeEncoderClick = 0;
    ClickEncoder *encoder;
    int16_t encoderValue = 0;
    int16_t encoderLastValue = 0;
    bool encoderBtnIsHeld = false;
#else
    Button buttonLeft(BUTTON_LEFT);
    Button buttonOK(BUTTON_OK);
    Button buttonRight(BUTTON_RIGHT);
#endif

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{nullptr};

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

ScreenManager *sm;
WidgetSet *widgetSet;

// This function should probably be moved somewhere else
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    // Dim bitmap
    for (int i = 0; i < w * h; i++) {
        bitmap[i] = Utils::rgb565dim(bitmap[i], sm->getBrightness(), true);
    }
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}


#ifndef USE_ROTARY_ENCODER
    /**
     * The ISR handlers must be static
     */
    void isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
    void isrButtonChangeMiddle() { buttonOK.isrButtonChange(); }
    void isrButtonChangeRight() { buttonRight.isrButtonChange(); }
#endif

void setupButtons() {
    #ifdef USE_ROTARY_ENCODER
        encoder = new ClickEncoder(ROTARY_CLK, ROTARY_DT, ROTARY_SW);
    #else
        buttonLeft.begin();
        buttonOK.begin();
        buttonRight.begin();

        attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), isrButtonChangeLeft, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BUTTON_OK), isrButtonChangeMiddle, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), isrButtonChangeRight, CHANGE);
    #endif
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    TJpgDec.setSwapBytes(true); // JPEG rendering setup
    TJpgDec.setCallback(tft_output);
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

    TJpgDec.setJpgScale(1);
    TJpgDec.drawJpg(0, 0, logo_start, logo_end - logo_start);

    widgetSet = new WidgetSet(sm);

#ifdef GC9A01_DRIVER
    Serial.println("GC9A01 Driver");
#endif
#if HARDWARE == WOKWI
    Serial.println("Wokwi Build");
#endif

    pinMode(BUSY_PIN, OUTPUT);
    Serial.println("Connecting to WiFi");

    wifiWidget = new WifiWidget(*sm);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();

    widgetSet->add(new ClockWidget(*sm));
#ifdef PARQET_PORTFOLIO_ID
    widgetSet->add(new ParqetWidget(*sm));
#endif
#ifdef STOCK_TICKER_LIST
    widgetSet->add(new StockWidget(*sm));
#endif
    widgetSet->add(new WeatherWidget(*sm));
#ifdef WEB_DATA_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, WEB_DATA_WIDGET_URL));
#endif
#ifdef WEB_DATA_STOCK_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, WEB_DATA_STOCK_WIDGET_URL));
#endif
#ifdef MQTT_WIDGET_HOST
    widgetSet->add(new MQTTWidget(*sm, MQTT_WIDGET_HOST, MQTT_WIDGET_PORT));
#endif

    m_widgetCycleDelayPrev = millis();
}

void checkCycleWidgets() {
    if (m_widgetCycleDelay > 0 && (m_widgetCycleDelayPrev == 0 || (millis() - m_widgetCycleDelayPrev) >= m_widgetCycleDelay)) {
        widgetSet->next();
        m_widgetCycleDelayPrev = millis();
    }
}

#ifdef USE_ROTARY_ENCODER

void performEncoderService() {
    if (micros() - lastTimeEncoderService < 500)
        return;

    lastTimeEncoderService = micros();
    encoder->service();
}

void handleEncoderTurn() {
    encoderValue += encoder->getValue();

    if (millis() - lastTimeEncoderTurn < 1000)
       return;

    if (encoderLastValue < encoderValue) {
        Serial.println("rotary left -> switch to prev Widget");
        m_widgetCycleDelayPrev = millis();
        widgetSet->prev();
    }
    else if (encoderLastValue > encoderValue) {
        Serial.println("rotary right -> switch to next Widget");   
        m_widgetCycleDelayPrev = millis();
        widgetSet->next();
    }

    encoderLastValue = encoderValue;
    lastTimeEncoderTurn = millis();
}

void handleEncoderClick() {
    if (millis() - lastTimeEncoderClick < 100)
      return;

    ClickEncoder::Button encoderButton = encoder->getButton();
    if (encoderButton == ClickEncoder::Clicked) {
        Serial.println("rotary click -> send OK short");        
        m_widgetCycleDelayPrev = millis();
        widgetSet->buttonPressed(BUTTON_OK, BTN_SHORT);
    }
    else if (encoderButton == ClickEncoder::DoubleClicked) {
        Serial.println("rotary dblclick -> send OK medium");        
        m_widgetCycleDelayPrev = millis();
        widgetSet->buttonPressed(BUTTON_OK, BTN_MEDIUM);

    }
    else if (!encoderBtnIsHeld && encoderButton == ClickEncoder::Held ) {
        Serial.println("rotary held start - send OK long");
        encoderBtnIsHeld = true;     
        m_widgetCycleDelayPrev = millis();   
        widgetSet->buttonPressed(BUTTON_OK, BTN_LONG);
    }
    else if (encoderBtnIsHeld && encoderButton == ClickEncoder::Released ) {
        Serial.println("rotary held end");
        encoderBtnIsHeld = false;
    }

    lastTimeEncoderClick = millis();
  }

void checkButtons() {
    performEncoderService();
    handleEncoderTurn();
    handleEncoderClick();
}

#else

void checkButtons() {
    // Reset cycle timer whenever a button is pressed
    if (buttonLeft.pressedShort()) {
        // Left short press cycles widgets backward
        Serial.println("Left button short pressed -> switch to prev Widget");
        m_widgetCycleDelayPrev = millis();
        widgetSet->prev();
    } else if (buttonRight.pressedShort()) {
        // Right short press cycles widgets forward
        Serial.println("Right button short pressed -> switch to next Widget");
        m_widgetCycleDelayPrev = millis();
        widgetSet->next();
    } else {
        ButtonState leftState = buttonLeft.getState();
        ButtonState middleState = buttonOK.getState();
        ButtonState rightState = buttonRight.getState();

        // Everying else that is not BTN_NOTHING will be forwarded to the current widget
        if (leftState != BTN_NOTHING) {
            Serial.printf("Left button pressed, state=%d\n", leftState);
            m_widgetCycleDelayPrev = millis();
            widgetSet->buttonPressed(BUTTON_LEFT, leftState);
        } else if (middleState != BTN_NOTHING) {
            Serial.printf("Middle button pressed, state=%d\n", middleState);
            m_widgetCycleDelayPrev = millis();
            widgetSet->buttonPressed(BUTTON_OK, middleState);
        } else if (rightState != BTN_NOTHING) {
            Serial.printf("Right button pressed, state=%d\n", rightState);
            m_widgetCycleDelayPrev = millis();
            widgetSet->buttonPressed(BUTTON_RIGHT, rightState);
        }
    }
}

#endif

void loop() {
    if (wifiWidget->isConnected() == false) {
        wifiWidget->update();
        wifiWidget->draw();
        widgetSet->setClearScreensOnDrawCurrent(); // Clear screen after wifiWidget
        delay(100);
    } else {
        if (!widgetSet->initialUpdateDone()) {
            widgetSet->initializeAllWidgetsData();
        }
        globalTime->updateTime();

        checkButtons();

        widgetSet->updateCurrent();
        widgetSet->updateBrightnessByTime(globalTime->getHour24());
        widgetSet->drawCurrent();

        checkCycleWidgets();
    }
}
