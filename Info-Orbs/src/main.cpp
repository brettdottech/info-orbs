#include <Arduino.h>
#include "user.h"
#include <WiFi.h>
#include "screenManager.h"
#include "widgets/clockWidget.h"

TFT_eSPI tft = TFT_eSPI();
ScreenManager* sm; // Initialize the screen manager and the displays
ClockWidget* clockWidget; // Initialize the clock widget

void setup() {

  Serial.begin(115200);
  Serial.println("Starting up...");

  // Initialize the display

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to the WiFi network");

  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_BLACK);

  clockWidget = new ClockWidget(*sm);
  clockWidget->draw();
}

void loop() {
  clockWidget->update();
  clockWidget->draw();

  delay(1000);
}
