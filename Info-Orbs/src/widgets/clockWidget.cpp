#include "widgets/clockWidget.h"
#include "user.h"
#include <TimeLib.h>

ClockWidget::ClockWidget(ScreenManager &manager): Widget(manager) {
    m_timeClient = new NTPClient(m_udp);
    m_timeClient->begin();
    m_timeClient->setPoolServerName(NTP_SERVER);
    // m_timeClient->setUpdateInterval(6000); // default is 6000
    //m_timeClient->update();
    //m_unixEpoch = m_timeClient->getEpochTime();
    m_timeZoneOffset = 0;
}

ClockWidget::~ClockWidget() {
    delete m_timeClient;
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
}

void ClockWidget::update() {
    this->timeUpdate();
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

void ClockWidget::timeUpdate() {
    if(millis() - m_updateTimer > m_oneSecond) {
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_updateTimer = millis();

        m_minuteSingle = minute(m_unixEpoch);
        #if FORMAT_24_HOUR == true
        m_hourSingle = hour(m_unixEpoch);
        #else
        m_hourSingle = hourFormat12(m_unixEpoch);
        #endif
    }
}

//TODO: Ask what Font is
void ClockWidget::displayDidget(int displayIndex, String didget, int font, int fontSize, uint32_t color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& display = m_manager.getDisplay();
    display.setTextSize(fontSize);
    if(SHADOWING && font == 7){
        display.setTextColor(BG_COLOR, TFT_BLACK);
        display.drawString("8", SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
        display.setTextColor(color);
    } else {
        display.setTextColor(color, TFT_BLACK);
    }
    display.drawString(didget, SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
}
