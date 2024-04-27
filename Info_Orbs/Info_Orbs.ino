#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>
#include "icons.h"

TFT_eSPI tft = TFT_eSPI();
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Constant/quality of life vars
#define bgcl 0x20a1        // clock shadow colour(Light brown)
#define lightOrange 0xfc80 // orange for clock
int centre = 120;          // centre location of the screen(240x240)

// uncomment these if youre using wokwi simulated dev environment
// #define TFT_DC 2
// #define TFT_MOSI 23
// #define TFT_SCLK 18
// const char *ssid = "Wokwi-GUEST";
// const char *password = "";

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++ CONFIG, CHANGE THESE BEFORE BOOTING +++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

String weatherLocation = "Victoira,BC"; // Change this to your currently location in format "CITY,STATECODE"
String weatherUnits = "metric";         // Units for eeather "us" for F,  "metric" for C
// #include "credentials.h"
const char *ssid = "";                               // Wifi U
const char *password = "";                           // Wifi P
String stocks[]{"AMD", "VT", "AAPL", "SPY", "META"}; // These are the stocks you will be tracking on the stock widget(s
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Other deffinitons/for pins/specific app actions
int pins[]{13, 33, 32, 25, 21};                 // Screen CS pins (pins that dictate which display is being writeen to)
int pinArrLen = sizeof(pins) / sizeof(pins[0]); // # of pins caclualtion
int buttonPin = 26;                             // button pin

// msic state tracking varibales
bool shadowing = true; // Enable/disable text shadow, this is used in the 7 segment display font
bool midon = true;     // monitor state of flashing cursor on screen
int appCount = 0;      // to track which screen in displayed
bool lastButtonState;  // tracks whether the button is pressed
bool buttonState = 0;  // Monitor button press

// Global Variables to track/store weather data
String cityName;
String currentWeatherText; // Weather Description
String currentWeatherIcon; // Text refrence for weather icon
String currentWeatherDeg;
String daysIcons[3]; // Icons/Temp For Next 3 days
String daysDegs[3];

// Time/Clock Stuff
unsigned long unix_epoch; // Current time unix
int timeZoneOffSet = 0;   // timezone offset pulled from the wetaher API
                          // These are broken out variables for the current time stored as strings to allow for ease of displaying on screeen without haveing to parse each time.
String hourSingle;        // current hour(12 hour)
String minuteSingle;      // current minute
String daySingle;         // current day (number)
String monthSingle;       // current month text (number can be got aswell, just havent needed it yet)
String weekSingle;        // current week text (number can be got aswell, just havent needed it yet)
String hd1;               // first & secon didget of hour & min
String hd2;
String md1;
String md2;

String last_minute; // variable to store the last minute the time API picked up, this is checked to know when to refresh the clock
String last_hour;   // variable to store the last hour the time API picked up
String last_day;    // variable to store the last day the time API picked up

// Delays for setting how often certain screens/functions are refreshed/checked. These include both the frequency which they need to be checked and a varibale to store the last checked value.
long secondTimer = 1000; // this time is used to refressh/check the clock every second.
long secondTimerPrev = 0;

const long semiColonFlash = 1500; // dealy for colon blink on clock widget
unsigned long semiColonFlashPrev = 0;

const long stockDelay = 120000; // stock refresh rate
unsigned long stockDelayPrev = 120000;

const long weatherDelay = 600000; // weather refresh rate
unsigned long weatherDelayPrev = 600000;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++ DEFINE WIDGETS TO BE DISPLAYED ON SCREE+++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Basic CLock, shows the time displayed across all 5 screens, each screen containing a charter. middle screen is a flashing semicolon
void clockWid()
{ // This widget shows the time displayed across all 5 screens, each screen containing a charter. middle screen is a flashing semicolon
  timeUpdate();
  if (last_hour != hourSingle)
  { // update hour if hour value has changed, ensure any numbers are offset to the correct displays
    if (hd2 == "")
    {
      displayDidget(0, "", 7, 5, lightOrange);
      displayDidget(1, hd1, 7, 5, lightOrange);
    }
    else
    {
      displayDidget(0, hd1, 7, 5, lightOrange);
      displayDidget(1, hd2, 7, 5, lightOrange);
    }
    last_hour = hourSingle;
  }

  if (millis() - semiColonFlashPrev >= semiColonFlash)
  { // flash semicolon every 1.5s
    if (midon == true)
    {
      shadowing = false;
      displayDidget(2, ":", 7, 5, lightOrange);
      midon = false;
    }
    else
    {
      shadowing = false;
      displayDidget(2, ":", 7, 5, bgcl);
      midon = true;
    }
    shadowing = true;
    semiColonFlashPrev = millis();
  }
  if (last_minute != minuteSingle)
  { // update minute if minute value has changed, ensure any numbers are offset to the correct displays
    if (md2 == "")
    {
      displayDidget(3, "0", 7, 5, lightOrange);
      displayDidget(4, md1, 7, 5, lightOrange);
    }
    else
    {
      displayDidget(3, md1, 7, 5, lightOrange);
      displayDidget(4, md2, 7, 5, lightOrange);
    }
    last_minute = minuteSingle;
  }
}

// Stocks, shows 5 stock tickers on the screen
void displayStocks()
{
  if (millis() - stockDelayPrev >= stockDelay)
  {
    for (int i = 0; i < 5; i++)
    {
      singleStockScreen(stocks[i], i);
    }
    stockDelayPrev = millis();
  }
}

// Weather, displays a clock, city & text weather discription, weather icon, temp, 3 day forecast
void weatherWid()
{
  singleClockScreen(0, TFT_BLACK, TFT_WHITE);
  if (millis() - weatherDelayPrev >= weatherDelay)
  {
    getWeatherData();
    weatherText(1, TFT_WHITE, TFT_BLACK);
    drawWeatherIcon(currentWeatherIcon, 2, 0, 0, 1);
    singleWeatherDeg(3, TFT_WHITE, TFT_BLACK);
    threeDayWeather(4);
    weatherDelayPrev = millis();
  }
}

//=======================Functions End===================================================================

void setup(void)
{

  // loop through all displays cs ping and set pins to output and write each display to 0(writeable) in order to be initialized
  for (int i = 0; i < pinArrLen; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 0);
  }

  pinMode(buttonPin, INPUT_PULLDOWN); // Set the button pin

  // Set all the baseline display stuff, then deinit all the pins
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < pinArrLen; i++)
  {
    digitalWrite(pins[i], 1);
  }

  // connect to wifi stuff
  Serial.begin(9600);
  delay(2000);
  WiFi.begin(ssid, password);
  displayDidget(2, "connecting to wifi.", 1, 2, TFT_WHITE);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  clearDsp();

  getWeatherData();                                // pull a weather API payload to get the timezone from the users city
  timeClient.begin();                              // begin time client
  timeClient.setTimeOffset(3600 * timeZoneOffSet); // Set Time Zone offset
  lastButtonState = digitalRead(buttonPin);
  TJpgDec.setSwapBytes(true); // jpeg rendering setup
  TJpgDec.setCallback(tft_output);
}

// Check if button pushed, if so, cycle to next widget.
void loop()
{
  buttonState = digitalRead(buttonPin);
  if (lastButtonState != buttonState)
  {
    if (appCount < 2 && buttonState == 1)
    {
      appCount++;
      resetTime();
      clearDsp();
    }
    else if (buttonState == 1)
    {
      appCount = 0;
      resetTime();
      clearDsp();
    }
    lastButtonState = buttonState;
  }
  if (appCount == 0)
  {
    clockWid();
  }
  else if (appCount == 1)
  {

    weatherWid();
  }
  else if (appCount == 2)
  {
    displayStocks();
  }
}
