#include <Arduino.h>
#include "user.h"
#include <WiFi.h>
#include "display.h"



void setup()
{

  Serial.begin(115200);
  Serial.println("Starting up...");

  // Initialize the display
  TFT_eSPI tft = TFT_eSPI();
  Display display(tft);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}

void loop()
{
}
