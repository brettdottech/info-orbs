#include "core/wifiWidget.h"
#include "widgetSet.h"
#include "screenManager.h"
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"
#include "widgets/webDataWidget.h"
#include <Arduino.h>
#include <Button.h>
#include <globalTime.h>
#include <config.h>
#ifdef STOCK_TICKER_LIST
  #include <widgets/stockWidget.h>
#endif

TFT_eSPI tft = TFT_eSPI();

#ifdef WIDGET_CYCLE_DELAY
unsigned long m_widgetCycleDelay = WIDGET_CYCLE_DELAY;  // Automatically cycle widgets every X ms, set to 0 to disable
#else
unsigned long m_widgetCycleDelay = 0;
#endif
unsigned long m_widgetCycleDelayPrev = 0;

Button buttonOK(BUTTON_OK);
Button buttonLeft(BUTTON_LEFT);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{ nullptr };

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

ScreenManager* sm;
WidgetSet* widgetSet;

void setup() {

  buttonLeft.begin();
  buttonOK.begin();
  buttonRight.begin();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_WHITE);
  sm->reset();
  widgetSet = new WidgetSet(sm);

  TJpgDec.setSwapBytes(true); // JPEG rendering setup
  TJpgDec.setCallback(tft_output);

#ifdef GC9A01_DRIVER
  Serial.println("GC9A01 Driver");
#endif
#ifdef ILI9341_DRIVER
  Serial.println("ILI9341 Driver");
#endif
#if HARDWARE == WOKWI
  Serial.println("Wokwi Build");
#endif

  pinMode(BUSY_PIN, OUTPUT);
  Serial.println("Connecting to: " + String(WIFI_SSID));

  wifiWidget = new WifiWidget(*sm);
  wifiWidget->setup();

  globalTime = GlobalTime::getInstance();

  widgetSet->add(new ClockWidget(*sm));
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

  m_widgetCycleDelayPrev = millis();
}

void checkCycleWidgets() {
  if (m_widgetCycleDelay > 0 && (m_widgetCycleDelayPrev == 0 || (millis() - m_widgetCycleDelayPrev) >= m_widgetCycleDelay)) {
        widgetSet->next();
        m_widgetCycleDelayPrev = millis();
    }
}

void checkButtons() {
    if (buttonLeft.pressed()) {
      Serial.println("Left button pressed");
      m_widgetCycleDelayPrev = millis();
      widgetSet->prev();
    }
    if (buttonLeft.longPressed()) {
      Serial.println("Left button longpressed");
      widgetSet->buttonPressed(LEFT_BUTTON_PRESSED_LONG);
    }
    if (buttonOK.pressed()) {
      Serial.println("OK button pressed");
      widgetSet->buttonPressed(MIDDLE_BUTTON_PRESSED);
    }
    if (buttonOK.longPressed()) {
      Serial.println("OK button longpressed");
      widgetSet->buttonPressed(MIDDLE_BUTTON_PRESSED_LONG);
    }
    if (buttonRight.pressed()) {
      Serial.println("Right button pressed");
      m_widgetCycleDelayPrev = millis();
      widgetSet->next();
    }
    if (buttonRight.longPressed()) {
      Serial.println("Right button longpressed");
      widgetSet->buttonPressed(RIGHT_BUTTON_PRESSED_LONG);
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
    }
    globalTime->updateTime();

    checkButtons();

    widgetSet->updateCurrent();
    widgetSet->drawCurrent();

    checkCycleWidgets();
  }
}
