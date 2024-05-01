#include "widgets/clockWidget.h"
#include "user.h"
#include <TimeLib.h>

ClockWidget::ClockWidget(ScreenManager &manager): Widget(manager) {
    m_timeClient = new NTPClient(m_udp);
    m_timeClient->begin();
    m_timeClient->setPoolServerName(NTP_SERVER);
    // m_timeClient.setUpdateInterval(60000);
    m_timeClient->update();
    m_unixEpoch = m_timeClient->getEpochTime();
    m_timeZoneOffset = 0;
    m_updateTimer = millis();
}

ClockWidget::~ClockWidget() {
    delete m_timeClient;
}
void ClockWidget::draw() {
    if(m_prevDisplay1Didget != m_display1Didget) {
        displayDidget(0, m_display1Didget, 7, 5, LIGHT_ORANGE);
    }
    if(m_prevDisplay2Didget != m_display2Didget) {
        displayDidget(1, m_display2Didget, 7, 5, LIGHT_ORANGE);
    }
    if(m_prevDisplay4Didget != m_display4Didget) {
        displayDidget(3, m_display4Didget, 7, 5, LIGHT_ORANGE);
    }
    if(m_prevDisplay5Didget != m_display5Didget) {
        displayDidget(4, m_display5Didget, 7, 5, LIGHT_ORANGE);
    }
}

void ClockWidget::update() {
    this->timeUpdate();
    Serial.println("Updating clock widget");
    String currentHourPadded = String(hour(m_unixEpoch)).length() == 1 ? "0" + String(hour(m_unixEpoch)) : String(hour(m_unixEpoch));
    String currentMinutePadded = String(minute(m_unixEpoch)).length() == 1 ? "0" + String(minute(m_unixEpoch)) : String(minute(m_unixEpoch));


    m_prevDisplay1Didget = m_display1Didget;
    m_display1Didget = currentHourPadded.substring(0, 1);
    m_prevDisplay2Didget = m_display2Didget;
    m_display2Didget = currentHourPadded.substring(1, 2);

    m_prevDisplay4Didget = m_display4Didget;
    m_display4Didget = currentMinutePadded.substring(0, 1);
    m_prevDisplay5Didget = m_display5Didget;
    m_display5Didget = currentMinutePadded.substring(1, 2);

    Serial.println("Hour[1]: " + m_display1Didget);
    Serial.println("Hour[2]: " + m_display2Didget);

    Serial.println("Minute[1]: " + m_display4Didget);
    Serial.println("Minute[2]: " + m_display5Didget);
}

void ClockWidget::timeUpdate() {
    if(millis() - m_updateTimer > m_oneSecond) {
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_unixEpoch = 1714525442;
        m_updateTimer = millis();

        m_lastHourSingle = m_hourSingle;
        m_lastMinuteSingle = m_minuteSingle;

        m_minuteSingle = minute(m_unixEpoch);
        m_hourSingle = hour(m_unixEpoch);
    }
}

//TODO: Ask what Font is
void ClockWidget::displayDidget(int displayIndex, String didget, int font, int fontSize, uint32_t color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& display = m_manager.getDisplay();
    display.setTextSize(fontSize);
    if(SHADOWING && font == 7){
        display.setTextColor(color, TFT_BLACK);
        display.drawString("8", SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
        display.setTextColor(color);
    } else {
        display.setTextColor(color, TFT_BLACK);
    }
    display.drawString(didget, SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
}
