#include "globalTime.h"

#include <TimeLib.h>
#include <config.h>

GlobalTime *GlobalTime::m_instance = nullptr;

GlobalTime::GlobalTime() {
    m_timeClient = new NTPClient(m_udp);
    m_timeClient->begin();
    m_timeClient->setPoolServerName(NTP_SERVER);
}

GlobalTime::~GlobalTime() {
    delete m_timeClient;
}

GlobalTime *GlobalTime::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new GlobalTime();
    }
    return m_instance;
}

void GlobalTime::updateTime() {
    if (millis() - m_updateTimer > m_oneSecond) {
        if (m_timeZoneOffset == -1) {
            getTimeZoneOffsetFromAPI();
        }
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_updateTimer = millis();
        m_minute = minute(m_unixEpoch);
#if FORMAT_24_HOUR == true
        m_hour = hour(m_unixEpoch);
#else
        m_hour = hourFormat12(m_unixEpoch);
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

String GlobalTime::getHourPadded() {
    if (m_hour < 10) {
        return "0" + String(m_hour);
    } else {
        return String(m_hour);
    }
}

int GlobalTime::getMinute() {
    return m_minute;
}

String GlobalTime::getMinutePadded() {
    if (m_minute < 10) {
        return "0" + String(m_minute);
    } else {
        return String(m_minute);
    }
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

#include <HTTPClient.h>  // Include the necessary header file

bool GlobalTime::isPM() {
    return hour(m_unixEpoch) >= 12;
}

void GlobalTime::getTimeZoneOffsetFromAPI() {
    HTTPClient http;
    http.begin(String(TIMEZONE_API_URL) + "?key=" + TIMEZONE_API_KEY + "&format=json&fields=gmtOffset&by=zone&zone=" + String(TIMEZONE_API_LOCATION));
    int httpCode = http.GET();

    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (!error) {
            m_timeZoneOffset = doc["gmtOffset"].as<int>();
            Serial.print("Timezone Offset from API: ");
            Serial.println(m_timeZoneOffset);
            m_timeClient->setTimeOffset(m_timeZoneOffset);
        } else {
            Serial.println("Deserialization error on timezone offset API response");
        }
    } else {
        Serial.println("Failed to get timezone offset from API");
    }
}