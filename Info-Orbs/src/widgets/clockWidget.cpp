#include "widgets/clockWidget.h"
#include <config.h>
#include <globalTime.h>
#include "widgets/clockWidget.h"


ClockWidget::ClockWidget(ScreenManager &manager): Widget(manager) {
}

ClockWidget::~ClockWidget() {

}

void ClockWidget::setup() {
    m_lastDisplay1Didget = "-1";
    m_lastDisplay2Didget = "-1";
    m_lastDisplay4Didget = "-1";
    m_lastDisplay5Didget = "-1";

}

void ClockWidget::draw() {
    if(m_lastDisplay1Didget != m_display1Didget) {
        displayDidget(0, m_display1Didget, 7, 5, LIGHT_ORANGE);
        m_lastDisplay1Didget = m_display1Didget;
    }
    if(m_lastDisplay2Didget != m_display2Didget) {
        displayDidget(1, m_display2Didget, 7, 5, LIGHT_ORANGE);
        m_lastDisplay2Didget = m_display2Didget;
    }
    if(m_lastDisplay4Didget != m_display4Didget) {
        displayDidget(3, m_display4Didget, 7, 5, LIGHT_ORANGE);
        m_lastDisplay4Didget = m_display4Didget;
    }
    if(m_lastDisplay5Didget != m_display5Didget) {
        displayDidget(4, m_display5Didget, 7, 5, LIGHT_ORANGE);
        m_lastDisplay5Didget = m_display5Didget;
    }

    if(m_secondSingle != m_lastSecondSingle) {
        if(m_secondSingle % 2 == 0) {
            displayDidget(2, ":", 7, 5, LIGHT_ORANGE, false);
        } else {
            displayDidget(2, " ", 7, 5, LIGHT_ORANGE, false);
        }
        //displaySeconds(2, m_secondSingle, m_minuteSingle % 2 == 0 ? LIGHT_ORANGE : BG_COLOR);
        m_lastSecondSingle = m_secondSingle;
    }
    
}

void ClockWidget::update() {
    if (millis() - m_secondTimerPrev < m_secondTimer) {
        return;
    }
    // Time::getHourAndMinute(m_hourSingle, m_minuteSingle);
    // Time::getSecond(m_secondSingle);

    GlobalTime* time = GlobalTime::getInstance();
    m_hourSingle = time->getHour();
    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();


    if(m_lastHourSingle != m_hourSingle) {
        String currentHourPadded = String(m_hourSingle).length() == 1 ? "0" + String(m_hourSingle) : String(m_hourSingle);

        m_display1Didget = currentHourPadded.substring(0, 1);
        m_display2Didget = currentHourPadded.substring(1, 2);

        m_lastHourSingle = m_hourSingle;
    }

    if(m_lastMinuteSingle != m_minuteSingle) {
        String currentMinutePadded = String(m_minuteSingle).length() == 1 ? "0" + String(m_minuteSingle) : String(m_minuteSingle);
        
        m_display4Didget = currentMinutePadded.substring(0, 1);
        m_display5Didget = currentMinutePadded.substring(1, 2);

        m_lastMinuteSingle = m_minuteSingle;
    }

}

void ClockWidget::displayDidget(int displayIndex, const String& didget, int font, int fontSize, uint32_t color, bool shadowing) {
    //m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& display = m_manager.getDisplay();
    display.fillScreen(TFT_BLACK);
    display.setTextSize(fontSize);
    if(shadowing && font == 7){
        display.setTextColor(BG_COLOR, TFT_BLACK);
        display.drawString("8", SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
        display.setTextColor(color);
    } else {
        display.setTextColor(color, TFT_BLACK);
    }
    display.drawString(didget, SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
}

void ClockWidget::displayDidget(int displayIndex, const String& didget, int font, int fontSize, uint32_t color) {
    this->displayDidget(displayIndex, didget, font, fontSize, color, SHADOWING);
}


// working on it
void ClockWidget::displaySeconds(int displayIndex, int seconds, int color){
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& display = m_manager.getDisplay(); 
    if(seconds < 30){
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds + 180, 6 * seconds + 180 + 6, color, TFT_BLACK);
    } else {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds - 180, 6 * seconds - 180 + 6, color, TFT_BLACK);
    }
}
