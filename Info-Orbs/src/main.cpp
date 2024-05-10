#include <Arduino.h>
#include <user.h>
#include "screenManager.h"
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

String connectingString{ "" };

Widget* widgets[COUNT_OF_WIDGETS];
int8_t currentWidget = 1;

int connectionTimer{ 0 };
const int connectionTimeout{ 10000 };
bool isConnected{ true };
String connectionStatus{ "" };

void connectionTimedOut() {
  switch(WiFi.status()){
    case WL_CONNECTED:
      connectingString = "Connected";
      break;
    case WL_NO_SSID_AVAIL:
      connectingString = "No SSID available";
      break;
    case WL_CONNECT_FAILED:
      connectingString = "Connection failed";
      break;
    case WL_IDLE_STATUS:
      connectingString = "Idle status";
      break;
    case WL_DISCONNECTED:
      connectingString = "Disconnected";
      break;
    default:
      connectingString = "Unknown";
      break;
  }

  Serial.println("Connection timed out");
  TFT_eSPI& display = sm->getDisplay();
  sm->selectScreen(0);
  display.fillScreen(TFT_BLACK);
  display.drawCentreString("Connection", 120, 80, 1);
  display.drawCentreString(connectingString, 120, 100, 1);
}

void setup() {

  buttonLeft.begin();
  buttonOK.begin();
  buttonRight.begin();

  Serial.begin(115200);
  Serial.println("Starting up...");
  Serial.println("Connecting to: " + String(WIFI_SSID));
  
  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_BLACK);
  sm->reset();

  TFT_eSPI& display = sm->getDisplay();

  sm->selectScreen(0);
  display.fillScreen(TFT_BLACK);
  display.setTextSize(2);
  display.setTextColor(TFT_WHITE);
  display.drawCentreString("Connecting" + connectingString, 120, 80, 1);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  sm->selectScreen(1);
  display.fillScreen(TFT_BLACK);
  display.drawCentreString("Connecting to", 120, 80, 1);
  display.drawCentreString("WiFi..", 120, 100, 1);
  display.drawCentreString(WIFI_SSID, 120, 130, 1);

  Serial.println("Connecting to WiFi..");
  sm->selectScreen(0);
  display.fillScreen(TFT_BLACK);
  display.drawCentreString("Connecting", 120, 80, 1);
  
  while (WiFi.status() != WL_IDLE_STATUS && isConnected) {
    Serial.print(".");
    connectingString += ".";
    if(connectingString.length() > 3) {
      connectingString = "";
    }
    
    display.fillRect(0, 100, 240, 100, TFT_BLACK);
    display.drawCentreString(connectingString, 120, 100, 1);

    connectionTimer += 500;
    if(connectionTimer > connectionTimeout) {
      isConnected = false;
      break;
    }
    delay(500);
  }


  if(!isConnected) {
    connectionTimedOut();
    return;
  }

  #ifdef GC9A01_DRIVER
    Serial.println("GC9A01 Driver");
  #endif
  #ifdef ILI9341_DRIVER
    Serial.println("ILI9341 Driver");
  #endif
  #ifdef WOKWI
    Serial.println("Wokwi Build");
  #endif

  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_BLACK);
  sm->reset();

  Serial.println();
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
   

  globalTime = GlobalTime::getInstance();

  widgets[0] = new ClockWidget(*sm);
  widgets[1] = new WeatherWidget(*sm);

}

void loop() {
  if(isConnected){
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
}