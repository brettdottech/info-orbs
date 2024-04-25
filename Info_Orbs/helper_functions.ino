
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

