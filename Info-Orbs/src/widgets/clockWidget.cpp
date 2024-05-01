#include "widgets/clockWidget.h"
#include "user.h"
#include <TimeLib.h>

ClockWidget::ClockWidget(const ScreenManager &manager): Widget(manager) {
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
void ClockWidget::draw() {}

void ClockWidget::update() {}

void ClockWidget::timeUpdate() {
    if(millis() - m_updateTimer > m_oneSecond) {
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_updateTimer = millis();

        m_minuteSingle = String(minute(m_unixEpoch));
        m_hourSingle = String(hour(m_unixEpoch));
        m_daySingle = String(day(m_unixEpoch));
        
        m_monthSingle = String(month(m_unixEpoch));
        m_weekSingle = String(weekday(m_unixEpoch));
    }
}
