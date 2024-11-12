#include "widgets/nautelWidget.h"

#include <config.h>
#include <globalTime.h>

NautelWidget::NautelWidget(ScreenManager& manager) : Widget(manager) {
}

NautelWidget::~NautelWidget() {
}

void NautelWidget::setup() {
    m_lastDisplay1Didget = "-1";
    m_lastDisplay2Didget = "-1";
    m_lastDisplay4Didget = "-1";
    m_lastDisplay5Didget = "-1";

}

void NautelWidget::draw(bool force) {
    GlobalTime* time = GlobalTime::getInstance();
    

//if (m_secondSingle != m_lastSecondSingle || force) {

        displaySeconds(0, m_lastSecondSingle, TFT_BLACK);
        displaySeconds(0, m_secondSingle, ALT_FOREGROUND_COLOR);  

        displayGauge(1, (float ((float) m_secondSingle/60.0)), 0, 1, FOREGROUND_COLOR);
        displayGauge(2, m_secondSingle * 2, 0, 120, TFT_GREEN);
        displayGauge(3, m_secondSingle * 3, 0, 180, TFT_ORANGE);
        displayGauge(4, m_secondSingle * 3 , 0, 180, TFT_RED);

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
    tft.setTextColor(ALT_FOREGROUND_COLOR);
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
