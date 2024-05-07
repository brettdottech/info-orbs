#include "globalTime.h"
#include <TimeLib.h>
#include "../../include/user.h"

GlobalTime *GlobalTime::m_instance = nullptr;

GlobalTime::GlobalTime() {
    m_timeClient = new NTPClient(m_udp);
    m_timeClient->begin();
    m_timeClient->setPoolServerName(NTP_SERVER);
    m_timeClient->setTimeOffset(3600 * (TIME_ZONE_OFFSET));
}

GlobalTime::~GlobalTime() {
    delete m_timeClient;
}

GlobalTime* GlobalTime::getInstance() {
    if(m_instance == nullptr) {
        m_instance = new GlobalTime();
    }
    return m_instance;
}

void GlobalTime::updateTime() {
  if (millis() - m_updateTimer > m_oneSecond) {
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

void GlobalTime::getHourAndMinute(int &hour, int &minute) {
    hour = m_hour;
    minute = m_minute;
}


int GlobalTime::getHour() {
    return m_hour;
}

int GlobalTime::getMinute() {
    return m_minute;
}

int GlobalTime::getSecond() {
    return m_second;
}

time_t GlobalTime::getUnixEpoch() {
    return m_unixEpoch;
}

int GlobalTime::getDay() {
    return m_day;
}

int GlobalTime::getMonth() {
    return m_month;
}

String GlobalTime::getMonthName() {
    return m_monthName;
}

int GlobalTime::getYear() {
    return m_year;
}

String GlobalTime::getTime() {
    return m_time;
}

String GlobalTime::getWeekday() {
    return m_weekday;
}




