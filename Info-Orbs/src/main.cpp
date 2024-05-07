#include <Arduino.h>
#include "screenManager.h"
#include "user.h"
#include <WiFi.h>
#include <globalTime.h>
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"
#include <Button.h>

TFT_eSPI tft = TFT_eSPI();
ScreenManager* sm = new ScreenManager(tft); // Initialize the screen manager and the displays

// Button states
bool lastButtonOKState = HIGH;
bool lastButtonLeftState = HIGH;
bool lastButtonRightState = HIGH;

Button buttonOK(BUTTON_OK);
Button buttonLeft(BUTTON_LEFT);
Button buttonRight(BUTTON_RIGHT);


#define COUNT_OF_WIDGETS 2
ClockWidget* clockWidget; // Initialize the clock widget
WeatherWidget* weatherWidget; // Initialize the weather widget

GlobalTime* globalTime; // Initialize the global time

Widget* widgets[COUNT_OF_WIDGETS];
int8_t currentWidget = 1;

void setup() {

  buttonLeft.begin();
  buttonOK.begin();
  buttonRight.begin();

  Serial.begin(115200);
  Serial.println("Starting up...");
  Serial.println("Connecting to: " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_ORANGE);
  sm->reset();

  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_GREEN);
  sm->reset();

  Serial.println();
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

 

  globalTime = GlobalTime::getInstance();

  widgets[0] = new ClockWidget(*sm);
  widgets[1] = new WeatherWidget(*sm);

  widgets[currentWidget]->setup();



}

void loop() {
  globalTime->updateTime();

  // if(buttonRight.pressed()) {
  //   Serial.println("Right button pressed");
  //   currentWidget++;
  //   currentWidget %= COUNT_OF_WIDGETS;
  //   sm->fillAllScreens(TFT_BLACK);
  //   widgets[currentWidget]->setup();
  // }
  // This is a BIG WIP

  widgets[currentWidget]->update();
  widgets[currentWidget]->draw();
  delay(100);
}