#include "core/wifiWidget.h"
#include "widgetSet.h"
#include "screenManager.h"
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"
#include <Arduino.h>
#include <Button.h>
#include <globalTime.h>
#include <config.h>
#include <widgets/screenWidgets/stockScreenWidget.h>

TFT_eSPI tft = TFT_eSPI();
ScreenManager* sm = new ScreenManager(tft); // Initialize the screen manager and the displays
WidgetSet* widgetSet = new WidgetSet(sm);

// Button states
bool lastButtonOKState = HIGH;
bool lastButtonLeftState = HIGH;
bool lastButtonRightState = HIGH;

Button buttonOK(BUTTON_OK);
Button buttonLeft(BUTTON_LEFT);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};


WifiWidget *wifiWidget{ nullptr };

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

StockScreenWidget *stockScreenWidget;


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void setup() {

  buttonLeft.begin();
  buttonOK.begin();
  buttonRight.begin();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_WHITE);
  sm->reset();

  TJpgDec.setSwapBytes(true); // jpeg rendering setup
  TJpgDec.setCallback(tft_output);

#ifdef GC9A01_DRIVER
  Serial.println("GC9A01 Driver");
#endif
#ifdef ILI9341_DRIVER
  Serial.println("ILI9341 Driver");
#endif
#ifdef WOKWI
  Serial.println("Wokwi Build");
#endif


  Serial.println("Connecting to: " + String(WIFI_SSID));

  wifiWidget = new WifiWidget(*sm);
  wifiWidget->setup();

  globalTime = GlobalTime::getInstance();

  widgetSet->add(new ClockWidget(*sm));
  widgetSet->add(new WeatherWidget(*sm));

  // stockScreenWidget = new StockScreenWidget(*sm, "AAPL", 1);
}

void loop() {
  if (wifiWidget->isConnected() == false) {
    wifiWidget->update();
    wifiWidget->draw();
    delay(100);
  } else {
    globalTime->updateTime();

    if(buttonLeft.pressed()) {
      Serial.println("Left button pressed");
      widgetSet->prev();
      sm->clearAllScreens();
    }
    if(buttonOK.pressed()) {
      Serial.println("OK button pressed");
    }
    if(buttonRight.pressed()) {
      Serial.println("Right button pressed");
      widgetSet->next();
    }

    widgetSet->updateCurrent();
    widgetSet->drawCurrent();

    // stockScreenWidget->update();
    // stockScreenWidget->draw();
    delay(100);
  }
}
