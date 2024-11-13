#include "widgets/nautelWidget.h"
#include <stdlib.h>
#include <config.h>
#include <globalTime.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

WebSocketsClient webSocket;
JsonDocument doc();
 String stationName = "Listening";
 String frequency = "Dailing";
 String ip = "dhcp";
 float fanspeed =  0.0;
 float heatsinktemp = 0.0;
 float peakmodulation = 0.0;
 float poweroutput = 0.0;
 float powerreflected = 0.0;
 String date = "epoch";
 float swr = 0.0;


NautelWidget::NautelWidget(ScreenManager& manager) : Widget(manager) {
}

NautelWidget::~NautelWidget() {
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

const char* json = (char*)payload;
JsonDocument doc;
deserializeJson(doc, json);

const String wsStationName = doc["stationName"]; // "KRYZ"
const String wsFrequency = doc["frequency"]; // "98.5 Mhz"
const String wsIp = doc["ip"]; // "192.168.10.12"
const float wsFanspeed = doc["fanspeed"]; // "7323.0"
const float wsHeatsinktemp = doc["heatsinktemp"]; // "24.01"
const float wsPeakmodulation = doc["peakmodulation"]; // "100.013"
const float wsPoweroutput = doc["poweroutput"]; // "11.023"
const float wsPowerreflected = doc["powerreflected"]; // "0.176"
const String wsDate = doc["date"]; // "2024-11-13 01:59:18.965820Z"
const float wsSwr = doc["swr"]; // "1.289"


// Serial.println(fanspeed_ws);
// Serial.println(ip)_ws;
 stationName = wsStationName;
 frequency = wsFrequency;
 ip = wsIp;
 fanspeed = wsFanspeed;
 heatsinktemp = wsHeatsinktemp;
 peakmodulation = wsPeakmodulation;
 poweroutput = wsPoweroutput;
 powerreflected = wsPowerreflected;
 date = wsDate;
 swr = wsSwr;
}


void NautelWidget::setup() {

    webSocket.beginSSL("ws.kryzradio.org", 443);
    // Set up the WebSocket onConnect callback
    webSocket.onEvent(webSocketEvent);

}

void NautelWidget::draw(bool force) {
    GlobalTime* time = GlobalTime::getInstance();
    

//if (m_secondSingle != m_lastSecondSingle || force) {

        displayGauge(0, fanspeed, 5000, 9000, FOREGROUND_COLOR);
        displayGauge(1, swr, 0, 5, FOREGROUND_COLOR);
        displayGauge(2, poweroutput, 0, 120, FOREGROUND_COLOR);
        displayGauge(3, heatsinktemp, 0, 100, FOREGROUND_COLOR);
        displayGauge(4, peakmodulation, 0, 110, FOREGROUND_COLOR);

        m_lastSecondSingle = m_secondSingle;

  //  }

}



void NautelWidget::displayRadioStation(uint32_t color) {
    GlobalTime* time = GlobalTime::getInstance();
    m_manager.selectScreen(2);
    TFT_eSPI& display = m_manager.getDisplay();
    display.setTextSize(2);
    display.setTextColor(color, TFT_BLACK, true);
    String radioStation = "KRYZ";
    display.drawString(radioStation, SCREEN_SIZE / 2, SCREEN_SIZE / 2, 1);
}


void NautelWidget::update(bool force) {
    // if (millis() - m_secondTimerPrev < m_secondTimer && !force) {
    //     return;
    // }

    GlobalTime* time = GlobalTime::getInstance();
    m_hourSingle = time->getHour();
    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();
        // Start Websocket
        webSocket.loop();

}

void NautelWidget::changeMode() {

}



void NautelWidget::displaySeconds(int displayIndex, int seconds, int color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    int displaySeconds = seconds;
    TFT_eSPI& display = m_manager.getDisplay();
  
 
    if (displaySeconds < 30) {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 50, 6 * displaySeconds + 180, 6 * displaySeconds + 180 + 6, color, TFT_BLACK);
    } else {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 50, 6 * displaySeconds - 180, 6 * displaySeconds - 180 + 6, color, TFT_BLACK);
    }
    
}


void NautelWidget::displayGauge(int displayIndex, float value, int minValue, int maxValue, int color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& tft = m_manager.getDisplay();
    float step = (float)(maxValue  - minValue)/ TOTAL_ANGLE;
    bool up = false;
    uint32_t activeColor = 0;

    //Tidy values
    if (value < minValue){value = minValue;}
    if (value > maxValue){value = maxValue;}
    if (lastValue[displayIndex] < minValue){lastValue[displayIndex] = minValue;}
    if (lastValue[displayIndex] > maxValue){lastValue[displayIndex] = maxValue;}

    // Move the needle in the right direction & prevent jumping  
    if (value > lastValue[displayIndex] & value > lastValue[displayIndex] - step){
        value = lastValue[displayIndex] + step;
        up = true;
    }
     if (value < lastValue[displayIndex] & value < lastValue[displayIndex] - step)
    {
        value = lastValue[displayIndex] - step;
        up = false;
    }

    //return if value is the same
    if(lastValue[displayIndex] == value){return;}

    // Calculate needle angle based on value
    float valuePercent = (float)(value - minValue) / (maxValue - minValue);

    

    // Calc the angles
    float needleAngle = START_ANGLE + (TOTAL_ANGLE * valuePercent);
    float needleRad = needleAngle * PI / 180.0;


    // Get the center of the screen
    int centerX = SCREEN_SIZE / 2 ;
    int centerY = SCREEN_SIZE / 2 ;
    
    // Calculate needle endpoints
    int needleX = centerX + NEEDLE_LENGTH * cos(needleRad);
    int needleY = centerY + NEEDLE_LENGTH * sin(needleRad);

    
    // Draw gauge outline
    tft.drawCircle(centerX, centerY, GAUGE_RADIUS, TFT_LIGHTGREY);
    
    // Draw tick marks
    for (int i = 0; i <= 10; i++) {
        float angle = START_ANGLE + (TOTAL_ANGLE * i / 10.0);
        float angleRad = angle * PI / 180.0;
        
        int innerX = centerX + (GAUGE_RADIUS - TICK_LENGTH) * cos(angleRad);
        int innerY = centerY + (GAUGE_RADIUS - TICK_LENGTH) * sin(angleRad);
        int outerX = centerX + GAUGE_RADIUS * cos(angleRad);
        int outerY = centerY + GAUGE_RADIUS * sin(angleRad);
        
        tft.drawLine(innerX, innerY, outerX, outerY, TFT_LIGHTGREY);
        
        // Draw labels for min, max and middle values
        if (i == 0 || i == 5 || i == 10) {
            float labelValue;
            if (i == 0) labelValue = minValue;
            else if (i == 5) labelValue = ((float)minValue + (float)maxValue) / 2.0;
            else labelValue = maxValue;
            
            // Position labels slightly inside the tick marks
            int labelX = centerX + (GAUGE_RADIUS - TICK_LENGTH - 15) * cos(angleRad);
            int labelY = centerY + (GAUGE_RADIUS - TICK_LENGTH - 15) * sin(angleRad);
            
            // Adjust text alignment based on position
            if (i == 0) { // Left side
                labelX -= 0;
            } else if (i == 10) { // Right side
                labelX -= 5;
            } else { // Middle
                labelX -= 0;
            }
            
            tft.setTextColor(TFT_WHITE);
            tft.setTextSize(2);
            // Only display floats if we have a value less than 10
            if (labelValue >= 10 || labelValue == 0 || i == 0 | i == 10){
            tft.drawNumber(labelValue, labelX, labelY);
            } else{
            tft.drawFloat(labelValue, 2, labelX, labelY);
            }
        }
    }

    // Do nothing if the needle has not moved. Stops flickering
    if(lastNeedleAngle[displayIndex] != -1 | lastNeedleAngle[displayIndex] != needleAngle){
    // Select color
    for (int i = 0; i <= 2; i++){
    if (value > colorLimits[displayIndex][i]) {activeColor = colorValues[displayIndex][i];};
    }

        if(up){
        tft.drawSmoothArc(centerX, centerY, 115, 60, lastNeedleAngle[displayIndex] -95, lastNeedleAngle[displayIndex] -92.5, TFT_BLACK, TFT_BLACK);
        }else{
        tft.drawSmoothArc(centerX, centerY, 115, 60, lastNeedleAngle[displayIndex] -87.5, lastNeedleAngle[displayIndex] -85, TFT_BLACK, TFT_BLACK);           
        }
        tft.drawSmoothArc(centerX, centerY, 115, 60, needleAngle -92.5, needleAngle -87.5, activeColor, TFT_BLACK);
    }

    // Draw center hub
    tft.drawCircle(centerX, centerY, 60, activeColor);
    
    // Display Units and Measures
    tft.setTextColor(color);
    tft.setTextSize(2);
    int valueWidth = 5 * String(value).length(); // Approximate width of the text
    tft.drawString(measure[displayIndex],centerX, SCREEN_SIZE - 80, 1);
    tft.drawString(units[displayIndex],centerX , SCREEN_SIZE - 30, 1);
    
    // Draw Value Erase last value the update new value
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(3);
    if(lastValue[displayIndex] < 10){
         tft.drawFloat(lastValue[displayIndex], 2, centerX, centerY);
    }else{
         tft.drawFloat(lastValue[displayIndex], 0, centerX, centerY);

    }

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    if(value < 10){
         tft.drawFloat(value, 2, SCREEN_SIZE / 2, SCREEN_SIZE / 2);
    }else{
         tft.drawFloat(value, 0, SCREEN_SIZE / 2, SCREEN_SIZE / 2);

    }

    lastNeedleAngle[displayIndex] = needleAngle;
    lastValue[displayIndex] = value;
}
