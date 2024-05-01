#include <Arduino.h>
#include "user.h"
#include <WiFi.h>
#include "screenManager.h"


void setup() {

  Serial.begin(115200);
  Serial.println("Starting up...");

  // Initialize the display
  TFT_eSPI tft = TFT_eSPI();
  ScreenManager sm(tft); // Initialize the screen manager and the displays

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected to the WiFi network");
}

void loop() {
}
