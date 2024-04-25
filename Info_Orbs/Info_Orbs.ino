#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>
TFT_eSPI tft = TFT_eSPI();
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


// JPG weather icons
#include "icons.h" 
#include "credentials.h" 

// Constant/quality of life vars
#define bgcl 0x20a1 // clock shadow colour(Light brown)
#define lightOrange 0xfc80  // orange for clock 
int centre = 120;            // centre location of the screen(240x240)


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++ CONFIG, CHANGE THESE BEFORE BOOTING +++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

String weatherLocation ="Victoira,BC"; //Change this to your currently location in format "CITY,STATECODE"
String weatherUnits ="metric"; // Units for eeather "us" for F,  "metric" for C
const char *ssid = "";  //Wifi U
const char *password = ""; //Wifi P
String stocks[]{"AMD","VT","AAPL","SPY","META"}; // These are the stocks you will be tracking on the stock widget(s
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// Other deffinitons/for pins/specific app actions 
int pins[]{ 13, 33, 32, 25, 21 };                // Screen CS pins (pins that dictate which display is being writeen to)
int pinArrLen = sizeof(pins) / sizeof(pins[0]);  //# of pins caclualtion
int buttonPin = 26;                              //button pin


//msic state tracking varibales
bool shadowing = true;       // Enable/disable text shadow, this is used in the 7 segment display font
bool midon = true;     // monitor state of flashing cursor on screen
int appCount = 0; // to track which screen in displayed
bool lastButtonState; //tracks whether the button is pressed
bool buttonState = 0;  //Monitor button press




//Global Variables to track/store weather data 
String cityName; 
String currentWeatherText; // Weather Description 
String currentWeatherIcon;// Text refrence for weather icon
String currentWeatherDeg;
String daysIcons[3]; //Icons/Temp For Next 3 days
String daysDegs[3];



// Time/Clock Stuff
  unsigned long unix_epoch ;  // Current time unix
  int timeZoneOffSet = 0;  // timezone offset pulled from the wetaher API
// These are broken out variables for the current time stored as strings to allow for ease of displaying on screeen without haveing to parse each time.
  String hourSingle; // current hour(12 hour)
  String minuteSingle; // current minute 
  String daySingle; // current day (number)
  String monthSingle; //current month text (number can be got aswell, just havent needed it yet)
  String weekSingle; //current week text (number can be got aswell, just havent needed it yet)
  String hd1;//first & secon didget of hour & min
  String hd2;
  String md1;
  String md2;

String last_minute;  // variable to store the last minute the time API picked up, this is checked to know when to refresh the clock
String last_hour;    // variable to store the last hour the time API picked up
String last_day;  // variable to store the last day the time API picked up

//Delays for setting how often certain screens/functions are refreshed/checked. These include both the frequency which they need to be checked and a varibale to store the last checked value.
long secondTimer = 1000;//this time is used to refressh/check the clock every second.
long secondTimerPrev = 0;

const long semiColonFlash= 1500; //dealy for colon blink on clock widget
unsigned long semiColonFlashPrev = 0; 


const long stockDelay = 120000; //stock refresh rate
unsigned long stockDelayPrev = 120000; 

const long weatherDelay = 600000; //weather refresh rate
unsigned long weatherDelayPrev = 600000; 


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++ BASIC HIGH LEVEL FUNCTIONS+++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


///function to select a display (0-4)
void selDsp(int pin) {
  digitalWrite(pins[pin], 0);
}

//function to deselect a display
void unSelDsp(int pin) {
  digitalWrite(pins[pin], 1);
}

//function to clear all displays
void clearDsp() {
  for (int i = 0; i < pinArrLen; i++) {
    digitalWrite(pins[i], 0);
  }
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < pinArrLen; i++) {
    digitalWrite(pins[i], 1);
  }
 
}


//This just populates weather data into global variables, does not display anything
void getWeatherData(){ 
  String httpRequestAddress = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/"+weatherLocation+"/next3days?key=XW2RDGD6XK432AF25BNK2A3C7&unitGroup="+weatherUnits+"&include=days,current&iconSet=icons1";
  HTTPClient http;
  int httpCode;
  http.begin(httpRequestAddress);
  httpCode = http.GET();
  DynamicJsonDocument doc(200);
  deserializeJson(doc, http.getString());
timeZoneOffSet = doc["tzoffset"];
cityName = doc["resolvedAddress"].as<String>(); 
currentWeatherDeg = doc["currentConditions"]["temp"].as<String>();
currentWeatherText = doc["days"][0]["description"].as<String>();
currentWeatherIcon = doc["currentConditions"]["icon"].as<String>();
daysIcons[0] = doc["days"][1]["icon"].as<String>();
daysDegs[0] = doc["days"][1]["temp"].as<String>();
daysIcons[1] = doc["days"][2]["icon"].as<String>();//I hade these set up on a loop but I ran out of DRAM and had some varibles returning Nulls removing the loop fixed it however there is allot to optomize here.
daysDegs[1] = doc["days"][2]["temp"].as<String>();// I think the API call is too heavy, may need to be filtered before calling using the arduinoJson library, which is built in.
daysIcons[2] = doc["days"][3]["icon"].as<String>();
daysDegs[2] = doc["days"][3]["temp"].as<String>();
http.end();
}


// Function needed to write images to the display as per the TTJPGDecoder Library
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

// This will wrie an image to the screen when called from a hex array. Pass in:
// Screen #, X, Y coords, Bye Array To Pass, the sizeof that array, scale of the image(1= full size, then multiples of 2 to scale down)
// getting the byte array size is very annoying as its computed on compile so you cant do it dynamicly.
void showJPG(int d, int x, int y, const byte a[], int z, int s){
  selDsp(d);
  TJpgDec.setJpgScale(s);
  uint16_t w = 0, h = 0;
  TJpgDec.getJpgSize(&w, &h, a, z);
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);
  TJpgDec.drawJpg(x, y, a, z);
  unSelDsp(d);
}

// This takes the text output form the weatehr API and maps it to arespective icon/byte aarray, then displays it,
void drawWeatherIcon(String N, int d, int x, int y, int s){
if(N =="partly-cloudy-night"){
showJPG(d,x,y, moonCloud, sizeof(moonCloud),s);
}else if(N == "partly-cloudy-day"){ 
showJPG(d,x,y,sunClouds,sizeof(sunClouds),s);
}else if(N == "clear-day"){ 
showJPG(d,x,y,sun,sizeof(sun),s);
}else if(N == "clear-night"){ 
showJPG(d,x,y,moon,sizeof(moon),s);
}else if(N == "snow	"){ 
showJPG(d,x,y,snow,sizeof(snow),s);
}else if(N == "rain"){ 
showJPG(d,x,y,rain,sizeof(rain),s);
}else if(N == "fog"){ 
showJPG(d,x,y,clouds,sizeof(clouds),s);
}else if(N == "wind"){ 
showJPG(d,x,y,clouds,sizeof(clouds),s);
}else if(N == "cloudy"){ 
showJPG(d,x,y,clouds,sizeof(clouds),s);
}
}


//Display a centeered text/number on screen(with built in shadow function for the 7 segment dispay font), values: (screen #, string to display, font to display, font size, font color)
void displayDidget(int x, String y, int z, int a, uint32_t c) {
  selDsp(x);
  tft.setTextSize(a);
  if (shadowing == true && z == 7) {
    tft.setTextColor(bgcl, TFT_BLACK);
    tft.drawString("8", centre , centre, z);
    tft.setTextColor(c);
  } else {
    tft.setTextColor(c, TFT_BLACK);
  }
  tft.drawString(y, centre, centre, z);
  unSelDsp(x);
}



//Function to update time, ran every second, breaks down in a bunch of useable variables. 
//This very likely dosent need to be updated every single second, but not sure how much it actually hurts perforamnce.
void timeUpdate(){
if (millis() - secondTimerPrev >= secondTimer){
timeClient.update();
unix_epoch = timeClient.getEpochTime();  
hourSingle = String(hourFormat12(unix_epoch));
minuteSingle = String(minute(unix_epoch));
daySingle = String(day(unix_epoch));
monthSingle = monthStr(month(unix_epoch));
weekSingle = dayStr(weekday(unix_epoch));
hd1 = hourSingle.substring(0, 1);
hd2 = hourSingle.substring(1, 2);
md1 = minuteSingle.substring(0, 1);
md2 = minuteSingle.substring(1, 2);

secondTimerPrev = millis();
}
}


//This refreshes the time/API delays. This is needed when switching screens as if you switch to a new display and its something that has a long
//refresh like the hour or stock/weather delay, it wont load as it dosnet think it needs to be called. this could be easily solved with some sort of
//cacheing but we're not there yet lol
void resetTime(){
last_minute = "foo";  
last_hour = "foo";    
last_day = "foo";  
stockDelayPrev = 10000000;
weatherDelayPrev = 10000000;
}





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++ FUNCTIONS FOR INDIVIDUAL SCREENS+++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// This will show a stock ticker on the screen and its price. pass in the stock to display and the display
void singleStockScreen(String s, int d){
  String httpRequestAddress = "https://finnhub.io/api/v1/quote?symbol=" + s + "&token=c1tjb52ad3ia4h4uee9g";
  HTTPClient http;
  int httpCode;
  http.begin(httpRequestAddress);
  httpCode = http.GET();
  if (httpCode > 0) {
    DynamicJsonDocument doc(200);
    String payload = http.getString();
    Serial.println(payload);
    deserializeJson(doc, payload);
    float previousClosePrice = doc["pc"];
    float currentPrice = doc["c"];
    float perChange = doc["dp"];
    String perString = String(perChange,2);
    String currentPricStr = String(currentPrice,2);
selDsp(d);
tft.setTextSize(4);
tft.fillScreen(TFT_BLACK);
tft.setTextColor(TFT_WHITE); 
tft.fillRect(0, 0, 240, 50, 0x0256);// rgb565 colors
  tft.drawString(s, centre, 27, 1);
  tft.setTextSize(5);
  tft.drawString("$" + currentPricStr, centre, 51 + tft.fontHeight(1), 1);
  if (perString.substring(0,1) == "-"){
  tft.setTextColor(TFT_RED, TFT_BLACK); 
  tft.fillTriangle(120, 220, 140, 185, 100, 185, TFT_RED);
}
else{
  tft.setTextColor(TFT_GREEN, TFT_BLACK); 
  tft.fillTriangle(120, 185, 140, 220, 100, 220, TFT_GREEN);
}
  tft.drawString(perString + "%" , (centre), 147, 1);
unSelDsp(d);
}
http.end();
}


//This will show a single screen clock with date. Pass in Screen #, background color, text color
  void singleClockScreen(int d, uint32_t b, uint32_t t){
  timeUpdate();
  int clky = 95; 
  selDsp(d);
  tft.setTextColor(t); 
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
if (last_day != daySingle) { 
  last_day = daySingle;
  tft.fillScreen(b);
  tft.setTextColor(t); 
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(monthSingle + " "+  daySingle,centre, 151, 2);
  tft.setTextSize(3);
  tft.drawString(weekSingle,centre, 178, 2);
  }
  if (last_hour != hourSingle) { 
  tft.setTextColor(t,b); 
  tft.setTextDatum(MR_DATUM);
  tft.setTextSize(1);
  if (hourSingle.length() == 2){
  tft.drawString(hourSingle,centre - 5, clky, 8);
  }
    else {
  tft.drawString("0" + hourSingle,centre - 5, clky, 8);
  }
  last_hour = hourSingle;
  }
 if (last_minute != minuteSingle) { 
  tft.setTextColor(t,b); 
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(1);
  if (minuteSingle.length() == 2){
  tft.drawString(minuteSingle,centre + 5, clky, 8);
  }else{
     tft.drawString("0" + minuteSingle,centre + 5, clky, 8);
  }
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(t); 
  tft.drawString(":",centre, clky, 8);
  last_minute  = minuteSingle;
 }
  unSelDsp(d);
}



//This displays the current weather temp on a single screen. Pass in display number, background color, text color
//dosent round deg, just removes all text after the decimil, should prob be fixed
void singleWeatherDeg(int d, uint32_t b, uint32_t t){
 selDsp(d);
 tft.fillScreen(b);
  tft.setTextColor(t); 
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  currentWeatherDeg.remove(currentWeatherDeg.indexOf(".",0));
  tft.drawString(currentWeatherDeg,centre, centre, 8);
  tft.setTextFont(8);
  tft.fillCircle(tft.textWidth(currentWeatherDeg)/2 + centre + 10, centre - tft.fontHeight(8)/2 , 15,t);
   tft.fillCircle(tft.textWidth(currentWeatherDeg)/2 + centre + 10, centre - tft.fontHeight(8)/2, 7, b);
unSelDsp(d);
}


//This displays the users current city and the text desctiption of the weather. Pass in display number, background color, text color
void weatherText(int x, int16_t b, int16_t t){
  //=== TEXT OVERFLOW ============================
//this takes a given string a and breaks it down in max x character long strings ensuring not to break it only at a space. 
//given the small width of the screens this will porbablly be needed to this project again so making sure to outline it 
//clearly as this should liekly eventually be turned into a fucntion. Before use the array size should be made to be dynamic.
//In this case its used for the weather text description 
String message = currentWeatherText + " ";
String messageArr[4];
int variableRangeS = 0;
int variableRangeE = 18;
for (int i = 0; i < 4; i++){
while(message.substring(variableRangeE-1,variableRangeE) != " "){
variableRangeE--;
}
messageArr[i] = message.substring(variableRangeS,variableRangeE);
variableRangeS = variableRangeE;
variableRangeE = variableRangeS + 18;
}
//=== OVERFLOW END ==============================
  selDsp(x);
  tft.fillScreen(b);
  tft.setTextColor(t); 
  tft.setTextSize(3);
  tft.setTextDatum(MC_DATUM);
  cityName.remove(cityName.indexOf(",",0));
  tft.drawString(cityName,centre, 80, 2);
  Serial.println(cityName);
  Serial.println(currentWeatherText);
  tft.setTextSize(2);
  tft.drawString(messageArr[0] ,centre, 120, x);
  tft.drawString(messageArr[1],centre, 140, x);
  tft.drawString(messageArr[2],centre, 160, x);
  tft.drawString(messageArr[3],centre, 180, x);
  unSelDsp(x);
}


//This displays the next 3 days weather forecast
void threeDayWeather(int x){
  selDsp(x);                      
  tft.fillScreen(TFT_WHITE);
    tft.setTextSize(2);
  drawWeatherIcon(daysIcons[0], x,  90-75, 47, 4);
    drawWeatherIcon(daysIcons[1], x,  90, 47, 4);
    drawWeatherIcon(daysIcons[2], x,  90+75, 47, 4);
  selDsp(x);
  tft.setTextColor(TFT_BLACK); 
  tft.drawLine(78, 0, 78, 240, TFT_BLACK);
  tft.drawLine(79, 0, 79, 240, TFT_BLACK);
  tft.drawLine(80, 0, 80, 240, TFT_BLACK);
  tft.drawLine(157, 0, 157, 240, TFT_BLACK);
  tft.drawLine(158, 0, 158, 240, TFT_BLACK);
  tft.drawLine(159, 0, 159, 240, TFT_BLACK);
  daysDegs[0].remove(daysDegs[0].indexOf(".",0)); // remove decimal from deg, this dosent round just removes it, should probally fix this and also store as a different var type
  tft.drawString(daysDegs[0],centre-75, 120, 2);
  tft.drawCircle (60 , centre - tft.fontHeight(2)/2 , 4, TFT_BLACK);

  String weekUpdate = dayStr(weekday(unix_epoch+86400)); //Adds 1 day to time 
    weekUpdate.remove(3);
  weekUpdate.toUpperCase();
  tft.drawString(weekUpdate,centre-75, 150, 2); 

  daysDegs[1].remove(daysDegs[1].indexOf(".",0));
  tft.drawString(daysDegs[1],centre, 120, 2);
    tft.drawCircle( 132 , centre - tft.fontHeight(2)/2 , 4, TFT_BLACK);
  weekUpdate = dayStr(weekday(unix_epoch+172800));
    weekUpdate.remove(3);
  weekUpdate.toUpperCase();
  tft.drawString(weekUpdate,centre, 150, 2); 

  daysDegs[2].remove(daysDegs[2].indexOf(".",0));
  tft.drawString(daysDegs[2],centre+75, 120, 2);
  weekUpdate = dayStr(weekday(unix_epoch+259200));
  weekUpdate.remove(3);
  weekUpdate.toUpperCase();
  tft.drawCircle(207, centre - tft.fontHeight(2)/2 , 4, TFT_BLACK); //Had a variable offset on these but broke so I removed it
  tft.drawString(weekUpdate,centre+75, 150, 2); 
  tft.fillRect(0,180, 240, 70,TFT_RED);
  tft.setTextColor(TFT_WHITE); 
  tft.drawString("Next 3 Days..",centre, 201, 1);
  unSelDsp(x);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++ FULL WIDGETS TO BE DISPLAYED ON SCREE+++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// Basic CLock, shows the time displayed across all 5 screens, each screen containing a charter. middle screen is a flashing semicolon
void clockWid() {  // This widget shows the time displayed across all 5 screens, each screen containing a charter. middle screen is a flashing semicolon
 timeUpdate();
  if (last_hour != hourSingle) { // update hour if hour value has changed, ensure any numbers are offset to the correct displays
    if (hd2 == "") {
      displayDidget(0, "", 7, 5, lightOrange);
      displayDidget(1, hd1, 7, 5, lightOrange);
    } else {
      displayDidget(0, hd1, 7, 5, lightOrange);
      displayDidget(1, hd2, 7, 5, lightOrange);
    }
    last_hour = hourSingle;
  }

if (millis() - semiColonFlashPrev >= semiColonFlash){ // flash semicolon every 1.5s
  if (midon == true) { 
    shadowing = false;
    displayDidget(2, ":", 7, 5, lightOrange);
    midon = false;
  } else {
    shadowing = false;
    displayDidget(2, ":", 7, 5, bgcl);
    midon = true;
  }
  shadowing = true;
semiColonFlashPrev = millis();
}
  if (last_minute != minuteSingle) {  // update minute if minute value has changed, ensure any numbers are offset to the correct displays
    if (md2 == "") {
      displayDidget(3, "0", 7, 5, lightOrange);
      displayDidget(4, md1, 7, 5, lightOrange);
    } else {
      displayDidget(3, md1, 7, 5, lightOrange);
      displayDidget(4, md2, 7, 5, lightOrange);
    }
    last_minute = minuteSingle;
  }

}


//Stocks, shows 5 stock tickers on the screen
void displayStocks(){
if (millis() - stockDelayPrev >= stockDelay){
for (int i = 0; i < 5; i++) {
singleStockScreen(stocks[i],i);
}
stockDelayPrev = millis();
}
}

//Weather, displays a clock, city & text weather discription, weather icon, temp, 3 day forecast
void weatherWid(){
singleClockScreen(0,TFT_BLACK,TFT_WHITE);
if (millis() - weatherDelayPrev >= weatherDelay){
getWeatherData();
weatherText(1,TFT_WHITE,TFT_BLACK);
drawWeatherIcon(currentWeatherIcon, 2, 0,0,1);
singleWeatherDeg(3,TFT_WHITE,TFT_BLACK);
threeDayWeather(4);
weatherDelayPrev = millis();
  }
}


//=======================Functions End===================================================================

void setup(void) {


  // loop through all displays cs ping and set pins to output and write each display to 0(writeable) in order to be initialized
  for (int i = 0; i < pinArrLen; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 0);
  }

  pinMode(buttonPin, INPUT_PULLDOWN);  // Set the button pin

  // Set all the baseline display stuff, then deinit all the pins
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < pinArrLen; i++) {
    digitalWrite(pins[i], 1);
  }

  //connect to wifi stuff
  Serial.begin(9600);
  delay(2000);
  WiFi.begin(ssid, password);
  displayDidget(2, "connecting to wifi.", 1, 2, TFT_WHITE);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  clearDsp();

  getWeatherData(); // pull a weather API payload to get the timezone from the users city
  timeClient.begin();                // begin time client
  timeClient.setTimeOffset(3600*timeZoneOffSet);  // Set Time Zone offset
  lastButtonState = digitalRead(buttonPin);
  TJpgDec.setSwapBytes(true);//jpeg rendering setup
  TJpgDec.setCallback(tft_output);
}



void loop() {
buttonState = digitalRead(buttonPin); 
if (lastButtonState != buttonState){
if (appCount < 2 && buttonState == 1){
  appCount++;
  resetTime();
  clearDsp();
}else if (buttonState == 1){
  appCount = 0;
  resetTime();
  clearDsp();
}
lastButtonState = buttonState;
}
if (appCount == 0){
  clockWid();
  }else if (appCount == 1){

        weatherWid();

  }else if (appCount == 2){
    displayStocks();
  }
}
