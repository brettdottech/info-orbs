#include <time.h>
#include <TimeLib.h>
#include "../../include/user.h"

time_t Time::m_unixEpoch = 0;
int Time::m_hour = 0;
int Time::m_minute = 0;
int Time::m_second = 0;

Time::Time() {
    m_timeClient = new NTPClient(m_udp);
    m_timeClient->begin();
    m_timeClient->setPoolServerName(NTP_SERVER);
    m_timeClient->setTimeOffset(3600 * (TIME_ZONE_OFFSET));
}

Time::~Time() {
    delete m_timeClient;
}

void Time::updateTime() {
    if(millis() - m_updateTimer > m_oneSecond) {
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_updateTimer = millis();
        m_minute = minute(m_unixEpoch);
        #if FORMAT_24_HOUR == true
            m_hour = hour(m_unixEpoch);
        #else
            phour = hourFormat12(m_unixEpoch);
        #endif
        m_second = second(m_unixEpoch);
    }
}

void Time::getHourAndMinute(int &hour, int &minute) {
    hour = m_hour;
    minute = m_minute;
}

void Time::getSecond(int &second) {
    second = m_second;
}

void Time::getUnixEpoch(time_t &epoch) {
    epoch = m_unixEpoch;
}


