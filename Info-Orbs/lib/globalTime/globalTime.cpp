#include <globalTime.h>
#include <TimeLib.h>
#include "../../include/user.h"

time_t Time::m_unixEpoch = 0;
int Time::m_hour = 0;
int Time::m_minute = 0;
int Time::m_second = 0;
int Time::m_day = 0;
int Time::m_month = 0;
String Time::m_monthName = "";
int Time::m_year = 0;
String Time::m_time = "";
String Time::m_weekday = "";


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

        m_day = day(m_unixEpoch);
        m_month = month(m_unixEpoch);
        m_monthName = monthStr(m_month);
        m_year = year(m_unixEpoch);
        m_weekday = dayStr(weekday(m_unixEpoch));
        m_time = String(m_hour) + ":" + (m_minute < 10 ? "0" + String(m_minute) : String(m_minute));

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

void Time::getDay(int &day) {
    day = m_day;
}

void Time::getMonth(int &month) {
    month = m_month;
}

void Time::getMonthName(String &monthName) {
    monthName = m_monthName;
}

void Time::getYear(int &year) {
    year = m_year;
}

void Time::getTime(String &time) {
    time = m_time;
}

void Time::getWeekday(String &weekday) {
    weekday = m_weekday;
}




