#include <Arduino.h>
#include "user.h"
#include <WiFi.h>
#include <time.h>
#include "screenManager.h"
#include "widgets/clockWidget.h"

TFT_eSPI tft = TFT_eSPI();
ScreenManager* sm; // Initialize the screen manager and the displays
Time* time; // Initialize the time object

#define COUNT_OF_WIDGETS 1
ClockWidget* clockWidget; // Initialize the clock widget

Widget* widgets[COUNT_OF_WIDGETS];
uint8_t currentWidget = 0;

void setup() {

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

  time = new Time();
  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_BLACK);
  sm->reset();

  widgets[0] = new ClockWidget(*sm);

}

void loop() {
  time->updateTime();
  widgets[currentWidget]->update();
  widgets[currentWidget]->draw();
}
