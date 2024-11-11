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
    //displayRadioStation(ALT_FOREGROUND_COLOR);

}

void NautelWidget::draw(bool force) {
    GlobalTime* time = GlobalTime::getInstance();
    
    // if (m_lastDisplay1Didget != m_display1Didget || force) {
    //     displayDidget(0, m_display1Didget, 7, 5, ALT_FOREGROUND_COLOR );
    //     m_lastDisplay1Didget = m_display1Didget;
    //     if (SHADOWING != 1 &&m_display1Didget == " ") {
    //         m_manager.clearScreen(0);
    //     }
    // }
    // if (m_lastDisplay2Didget != m_display2Didget || force) {
    //     displayDidget(1, m_display2Didget, 7, 5, ALT_FOREGROUND_COLOR );
    //     m_lastDisplay2Didget = m_display2Didget;
    // }
    // if (m_lastDisplay4Didget != m_display4Didget || force) {
    //     displayDidget(3, m_display4Didget, 7, 5, ALT_FOREGROUND_COLOR );
    //     m_lastDisplay4Didget = m_display4Didget;
    // }
    // if (m_lastDisplay5Didget != m_display5Didget || force) {
    //     displayDidget(4, m_display5Didget, 7, 5, ALT_FOREGROUND_COLOR );
    //     m_lastDisplay5Didget = m_display5Didget;
    // }

//     if (m_secondSingle != m_lastSecondSingle || force) {
//         if (m_secondSingle % 2 == 0) {
//             displayDidget(2, "KRYZ", 7, 5, ALT_FOREGROUND_COLOR, false);
//         } else {
//             displayDidget(2, "KRYZ", 7, 5, BG_COLOR, false);
//         }

if (m_secondSingle != m_lastSecondSingle || force) {
#if SHOW_SECOND_TICKS == true 
        displaySeconds(0, m_lastSecondSingle, TFT_BLACK);
        displaySeconds(0, m_secondSingle, ALT_FOREGROUND_COLOR);  
        // displaySeconds(1, m_lastSecondSingle, TFT_BLACK);
        // displaySeconds(1, m_secondSingle, ALT_FOREGROUND_COLOR);     
        // displaySeconds(2, m_lastSecondSingle, TFT_BLACK);
        // displaySeconds(2, m_secondSingle, ALT_FOREGROUND_COLOR);
        // displaySeconds(3, m_lastSecondSingle, TFT_BLACK);
        // displaySeconds(3, m_secondSingle, ALT_FOREGROUND_COLOR);     
        // displaySeconds(4, m_lastSecondSingle, TFT_BLACK);
        // displaySeconds(4, m_secondSingle, ALT_FOREGROUND_COLOR);
        displayGauge(1, m_secondSingle, 0, 60, FOREGROUND_COLOR);
        displayGauge(2, m_secondSingle * 2, 0, 120, TFT_GREEN);
        displayGauge(3, m_secondSingle * 3, 0, 180, TFT_ORANGE);
        displayGauge(4, m_secondSingle * 4 , 60, 240, TFT_RED);


#endif
        m_lastSecondSingle = m_secondSingle;

    }

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
    if (millis() - m_secondTimerPrev < m_secondTimer && !force) {
        return;
    }

    GlobalTime* time = GlobalTime::getInstance();
    m_hourSingle = time->getHour();

    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();

    if (m_lastHourSingle != m_hourSingle || force) {
        if (m_hourSingle < 10) {
            if (FORMAT_24_HOUR) {
                m_display1Didget = "0";
            } else {
                m_display1Didget = " ";
            }
        } else {
            m_display1Didget = int(m_hourSingle/10);
        }
        m_display2Didget = m_hourSingle % 10;

        m_lastHourSingle = m_hourSingle;
    }

    if (m_lastMinuteSingle != m_minuteSingle || force) {
        String currentMinutePadded = String(m_minuteSingle).length() == 1 ? "0" + String(m_minuteSingle) : String(m_minuteSingle);

        m_display4Didget = currentMinutePadded.substring(0, 1);
        m_display5Didget = currentMinutePadded.substring(1, 2);

        m_lastMinuteSingle = m_minuteSingle;
    }
}

void NautelWidget::changeMode() {

}



void NautelWidget::displaySeconds(int displayIndex, int seconds, int color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    int displaySeconds = seconds;
    TFT_eSPI& display = m_manager.getDisplay();
    if (seconds > 20 && seconds < 40){displaySeconds = 21;}
    // if (seconds < 40){displaySeconds = 39;}
    if (displaySeconds < 30) {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 50, 6 * displaySeconds + 180, 6 * displaySeconds + 180 + 6, color, TFT_BLACK);
    } else {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 50, 6 * displaySeconds - 180, 6 * displaySeconds - 180 + 6, color, TFT_BLACK);
    }
    
}

void NautelWidget::displayGauge(int displayIndex, int value, int minValue, int maxValue, int color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& tft = m_manager.getDisplay();
    // int centerX = (displayIndex % 2) * 160 + 80;
    // int centerY = (displayIndex / 2) * 128 + 64;
    int centerX = SCREEN_SIZE / 2 ;
    int centerY = SCREEN_SIZE / 2 ;

    //Tidy values
    if (value < minValue){value = minValue;}
    if (value > maxValue){value = maxValue;}

    // Draw gauge outline
    tft.drawCircle(centerX, centerY, GAUGE_RADIUS, TFT_WHITE);
    
    // Draw tick marks
    for (int i = 0; i <= 10; i++) {
        float angle = START_ANGLE + (TOTAL_ANGLE * i / 10.0);
        float angleRad = angle * PI / 180.0;
        
        int innerX = centerX + (GAUGE_RADIUS - TICK_LENGTH) * cos(angleRad);
        int innerY = centerY + (GAUGE_RADIUS - TICK_LENGTH) * sin(angleRad);
        int outerX = centerX + GAUGE_RADIUS * cos(angleRad);
        int outerY = centerY + GAUGE_RADIUS * sin(angleRad);
        
        tft.drawLine(innerX, innerY, outerX, outerY, TFT_WHITE);
        
        // Draw labels for min, max and middle values
        if (i == 0 || i == 5 || i == 10) {
            int labelValue;
            if (i == 0) labelValue = minValue;
            else if (i == 5) labelValue = (minValue + maxValue) / 2;
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
            tft.drawNumber(labelValue, labelX, labelY);
        }
    }
    
    // Calculate needle angle based on value
    float valuePercent = (float)(value - minValue) / (maxValue - minValue);
    float needleAngle = START_ANGLE + (TOTAL_ANGLE * valuePercent);
    float needleRad = needleAngle * PI / 180.0;
    
    // Calculate needle endpoints
    int needleX = centerX + NEEDLE_LENGTH * cos(needleRad);
    int needleY = centerY + NEEDLE_LENGTH * sin(needleRad);
    
    if(lastNeedleAngle[displayIndex] != -1){
    // Draw needle with thickness
    for (int i = -NEEDLE_WIDTH/2; i <= NEEDLE_WIDTH/2; i++) {
        float offsetRad = (lastNeedleAngle[displayIndex] + i) * PI / 180.0;
        int offsetX = centerX + NEEDLE_LENGTH * cos(offsetRad);
        int offsetY = centerY + NEEDLE_LENGTH * sin(offsetRad);
        tft.drawLine(centerX, centerY, offsetX, offsetY, TFT_BLACK);
       // tft.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 50, offsetX -180 , offsetY -180, TFT_BLACK, TFT_BLACK);

    }
            tft.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 115, 50, lastNeedleAngle[displayIndex] -92.5, lastNeedleAngle[displayIndex] -87.5, TFT_BLACK, TFT_BLACK);

    }

    // Draw needle with thickness
    for (int i = -NEEDLE_WIDTH/2; i <= NEEDLE_WIDTH/2; i++) {
        float offsetRad = (needleAngle + i) * PI / 180.0;
        int offsetX = centerX + NEEDLE_LENGTH * cos(offsetRad);
        int offsetY = centerY + NEEDLE_LENGTH * sin(offsetRad);
        tft.drawLine(centerX, centerY, offsetX, offsetY, color);
    }
        tft.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 115, 50, needleAngle -92.5, needleAngle -87.5, color, TFT_BLACK);
    
    // Draw center hub
    tft.fillCircle(centerX, centerY, 5, color);
    tft.drawCircle(centerX, centerY, 5, TFT_WHITE);
    
    // Display current value below gauge in the empty space
    tft.setTextColor(ALT_FOREGROUND_COLOR);
    tft.setTextSize(2);
    int valueWidth = 5 * String(value).length(); // Approximate width of the text
    //tft.drawNumber(value, centerX - valueWidth, centerY + GAUGE_RADIUS/2 + 20);
    tft.drawString("value",SCREEN_SIZE / 2, SCREEN_SIZE - 50, 1);
    tft.drawString("KRYZ",SCREEN_SIZE / 2, SCREEN_SIZE - 20, 1);
    lastNeedleAngle[displayIndex] = needleAngle;
}
