
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
