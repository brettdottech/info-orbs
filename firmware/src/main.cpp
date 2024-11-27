#include "core/wifiWidget.h"
#include "widgetSet.h"
#include "screenManager.h"
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"
#include "widgets/webDataWidget.h"
#include <Arduino.h>
#include <Button.h>
#include <globalTime.h>
#include <utils.h>
#include <icons.h>
#include <config_helper.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>


#ifdef STOCK_TICKER_LIST
  #include <widgets/stockWidget.h>
#endif
#ifdef PARQET_PORTFOLIO_ID
  #include <widgets/parqetWidget.h>
#endif


TFT_eSPI tft = TFT_eSPI();

#ifdef WIDGET_CYCLE_DELAY
unsigned long m_widgetCycleDelay = WIDGET_CYCLE_DELAY * 1000;  // Automatically cycle widgets every X seconds, set to 0 to disable
#else
unsigned long m_widgetCycleDelay = 0;
#endif
unsigned long m_widgetCycleDelayPrev = 0;

Button buttonLeft(BUTTON_LEFT);
Button buttonOK(BUTTON_OK);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{ nullptr };

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

ScreenManager* sm;
WidgetSet* widgetSet;

// This function should probably be moved somewhere else
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    // Dim bitmap
    for (int i=0; i < w*h; i++) {
      bitmap[i] = Utils::rgb565dim(bitmap[i], sm->getBrightness(), true);
    }
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

/**
 * The ISR handlers must be static
 */
void isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void isrButtonChangeMiddle() { buttonOK.isrButtonChange(); }
void isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void setupButtons() {
  buttonLeft.begin();
  buttonOK.begin();
  buttonRight.begin();

  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), isrButtonChangeLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_OK), isrButtonChangeMiddle, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), isrButtonChangeRight, CHANGE);
}

bool restart_orb{false};
AsyncWebServer server(80);
Preferences preferences;

const char* PARAM_INPUT_1 = "Timezone";
const char* PARAM_INPUT_2 = "Location";
const char* PARAM_INPUT_3 = "Stockticker";
const char* PARAM_INPUT_4 = "Widgetcycle";

Preferences htmlpref;
String html_timezone;
String html_location;
String html_stockticker;
String html_widgetcycle;

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Info-Orbs Configuration</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Timezone (current value %TIMEZONE_HTML%): <input type="text" name="Timezone">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Location (current value %LOCATION_HTML%): <input type="text" name="Location">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Stockticker (current value %STOCKTICKER_HTML%): <input type="text" name="Stockticker">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Widget Cycle in seconds (current value %WIDGETCYCLE_HTML% - set to 0 to disable): <input type="text" name="Widgetcycle">
    <input type="submit" value="Submit">
  </form><br>
  </body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void StoreData(const char* key, const char* val){
  preferences.begin("info-orbs",false);
  preferences.putString(key, val);
  preferences.end();
}

// Replaces placeholder with stored values
String processor(const String& var){
    if(var == "TIMEZONE_HTML"){
      if (html_timezone != "empty"){
          return html_timezone;  
      }
      else {
          return (TIMEZONE_API_LOCATION);
      }
    }
    else if(var == "LOCATION_HTML"){
      if (html_location != "empty"){
          return html_location;  
      }
      else {
          return (WEATHER_LOCATION);
      }
    }
    else if(var == "STOCKTICKER_HTML"){
      if (html_stockticker != "empty"){
          return html_stockticker;  
      }
      else {
          return (STOCK_TICKER_LIST);
      }
    }
    else if(var == "WIDGETCYCLE_HTML"){
      if (html_widgetcycle != "empty"){
          return html_widgetcycle;  
      }
      else {
          return (String(WIDGET_CYCLE_DELAY));
      }
    }
    return String();
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
  sm->drawCentreString("Info Orbs", ScreenCenterX, ScreenCenterY - 40, 22);
  sm->drawCentreString("by", ScreenCenterX, ScreenCenterY, 22);
  sm->drawCentreString("brett.tech", ScreenCenterX, ScreenCenterY + 40, 22);

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

  m_widgetCycleDelayPrev = millis();

  // Hold left button when connecting to power to reset preferences
  // these are stored by Preferences in the namespace info-orbs
  if (digitalRead(BUTTON_LEFT) == Button::PRESSED_LEVEL) {
    htmlpref.begin("info-orbs",false);
    htmlpref.clear();
    htmlpref.end();
  }

  htmlpref.begin("info-orbs",false);
  html_timezone = htmlpref.getString("timezone","empty");
  html_location = htmlpref.getString("weather_loc","empty");
  html_stockticker = htmlpref.getString("stockticker","empty");
  html_widgetcycle = htmlpref.getString("widgetcycle","empty");
  htmlpref.end();

  if (html_timezone == "empty"){
      StoreData("timezone",TIMEZONE_API_LOCATION);
  }
  if (html_location == "empty"){
      StoreData("weather_loc",WEATHER_LOCATION);
  }
  if (html_stockticker == "empty"){
      StoreData("stockticker",STOCK_TICKER_LIST);
  }
  if (html_widgetcycle == "empty"){
      int i_widgetcylce = WIDGET_CYCLE_DELAY;
      StoreData("widgetcycle",String(i_widgetcylce).c_str());
  }
  
  if (html_widgetcycle != "empty")
  {
      m_widgetCycleDelay = html_widgetcycle.toInt() * 1000;
  }

// Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      StoreData("timezone",inputMessage.c_str());
      restart_orb = true;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      StoreData("weather_loc",inputMessage.c_str());
      restart_orb = true;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      StoreData("stockticker",inputMessage.c_str());
      restart_orb = true;
    }
    // GET input4 value on <ESP_IP>/get?input4=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;
      StoreData("widgetcycle",inputMessage.c_str());
      restart_orb = true;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "<script> function goBack() {window.history.back();}setTimeout(goBack, 10000);</script>Data sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page or wait 10 seconds</a>");
                                     
  });
  server.onNotFound(notFound);
  server.begin();
}

void checkCycleWidgets() {
  if (m_widgetCycleDelay > 0 && (m_widgetCycleDelayPrev == 0 || (millis() - m_widgetCycleDelayPrev) >= m_widgetCycleDelay)) {
        widgetSet->next();
        m_widgetCycleDelayPrev = millis();
    }
}

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

    if (restart_orb == true){
      ESP.restart();
    }
  }
}
