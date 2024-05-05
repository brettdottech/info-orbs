#include <Arduino.h>
#include "user.h"
#include <WiFi.h>
#include <globalTime.h>
#include "screenManager.h"
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"

TFT_eSPI tft = TFT_eSPI();
ScreenManager* sm; // Initialize the screen manager and the displays
Time* globalTime; // Initialize the time object

// Button states
bool lastButtonOKState = HIGH;
bool lastButtonLeftState = HIGH;
bool lastButtonRightState = HIGH;

#define COUNT_OF_WIDGETS 2
ClockWidget* clockWidget; // Initialize the clock widget
WeatherWidget* weatherWidget; // Initialize the weather widget

Widget* widgets[COUNT_OF_WIDGETS];
int8_t currentWidget = 0;

void setup() {

  pinMode(BUTTON_OK, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Starting up...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to the WiFi network");

  globalTime = new Time();
  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_BLACK);
  sm->reset();

  widgets[0] = new ClockWidget(*sm);
  widgets[1] = new WeatherWidget(*sm);

}

void loop() {
  globalTime->updateTime();

  // Read button states
  bool currentButtonOKState = digitalRead(BUTTON_OK);
  bool currentButtonLeftState = digitalRead(BUTTON_LEFT);
  bool currentButtonRightState = digitalRead(BUTTON_RIGHT);

  // Flank detection for buttons
  if (currentButtonLeftState == LOW && lastButtonLeftState == HIGH) {
    currentWidget--;
    if (currentWidget < 0) {
      currentWidget = COUNT_OF_WIDGETS - 1;
    }
    sm->fillAllScreens(TFT_BLACK);
    widgets[currentWidget]->update(true);
    widgets[currentWidget]->draw(true);
  }
  if (currentButtonRightState == LOW && lastButtonRightState == HIGH) {
    currentWidget++;
    if (currentWidget >= COUNT_OF_WIDGETS) {
      currentWidget = 0;
    }
    sm->fillAllScreens(TFT_BLACK);
    widgets[currentWidget]->update(true);
    widgets[currentWidget]->draw(true);
  }

  // Update last button states
  lastButtonOKState = currentButtonOKState;
  lastButtonLeftState = currentButtonLeftState;
  lastButtonRightState = currentButtonRightState;

  widgets[currentWidget]->update();
  widgets[currentWidget]->draw();

  delay(100);
}