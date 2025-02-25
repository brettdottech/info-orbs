#include "GlobalTime.h"

#include "ConfigManager.h"
#include "config_helper.h"
#include <ArduinoJson.h>

GlobalTime *GlobalTime::m_instance = nullptr;

GlobalTime::GlobalTime() {

    ConfigManager *cm = ConfigManager::getInstance();
    m_zones[0].zone = cm->getConfigString("timezoneLoc0", m_zone0); // config added in MainHelper
    m_zones[0].cityName = cm->getConfigString("timezoneName0", m_cityName0);
    m_zones[1].zone = cm->getConfigString("timezoneLoc1", m_zone1);
    m_zones[1].cityName = cm->getConfigString("timezoneName1", m_cityName1);
    m_zones[2].zone = cm->getConfigString("timezoneLoc2", m_zone2);
    m_zones[2].cityName = cm->getConfigString("timezoneName2", m_cityName2);
    m_zones[3].zone = cm->getConfigString("timezoneLoc3", m_zone3);
    m_zones[3].cityName = cm->getConfigString("timezoneName3", m_cityName3);
    m_zones[4].zone = cm->getConfigString("timezoneLoc4", m_zone4);
    m_zones[4].cityName = cm->getConfigString("timezoneName4", m_cityName4);

    int clockFormat = cm->getConfigInt("clockFormat", CLOCK_FORMAT); // config added in ClockWidget
    m_ntpServer = cm->getConfigString("ntpServer", m_ntpServer); // config added in MainHelper
    Serial.printf("GlobalTime initialized, tzLoc=%s, clockFormat=%d, ntpServer=%s\n", m_zones[0].zone.c_str(), clockFormat, m_ntpServer.c_str());
    m_format24hour = (clockFormat == CLOCK_FORMAT_24_HOUR);
    m_timeClient = new NTPClient(m_udp, m_ntpServer.c_str());
    m_timeClient->begin();
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

void GlobalTime::updateTime(bool force) {
    if (force || millis() - m_updateTimer > m_oneSecond) {
        getTimeZoneOffsetFromAPI();
        m_timeClient->setUpdateInterval(600000);  // 10 mins in ms
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_updateTimer = millis();
        m_minute = minute(m_unixEpoch);
        if (m_format24hour) {
            m_hour = hour(m_unixEpoch);
        } else {
            m_hour = hourFormat12(m_unixEpoch);
        }
        m_hour24 = hour(m_unixEpoch);
        m_second = second(m_unixEpoch);

        m_day = day(m_unixEpoch);
        m_month = month(m_unixEpoch);
        m_monthName = LOC_MONTH[m_month - 1];
        m_year = year(m_unixEpoch);
        m_weekday = LOC_WEEKDAY[(weekday(m_unixEpoch)) -1];
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

int GlobalTime::getHour24() {
    return m_hour24;
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

String GlobalTime::getDayAndMonth() {
#ifdef WEATHER_UNITS_METRIC
    String retVal = LOC_FORMAT_DAYMONTH;
    retVal.replace("%d", String(m_day));
    retVal.replace("%B", m_monthName);
    return retVal;
#else
    return m_monthName + " " + String(m_day);
#endif
}

#include <HTTPClient.h> // Include the necessary header file

bool GlobalTime::isPM() {
    return hour(m_unixEpoch) >= 12;
}

int GlobalTime::getUTCoffset(int &zoneId){
    int utcOffset;
    utcOffset = m_zones[zoneId].utcOffset;
    return utcOffset;
}

void GlobalTime::setTZforTime(int &offSet){
    m_timeClient->setTimeOffset(offSet);
}

String GlobalTime::getZoneName(int &zoneId){
    String zoneName;
    zoneName = m_zones[zoneId].cityName.c_str();
    return zoneName;
}

void GlobalTime::getTimeZoneOffsetFromAPI() {
    HTTPClient http;
    for (int i = 0; i < MAX_ZONES; i++) {
        if (m_zones[i].zone != ""){
        if (!m_zones[i].tzLoaded || (m_zones[i].m_nextTimeZoneChange > 0 && m_zones[i].dstActive && m_unixEpoch > m_zones[i].m_nextTimeZoneChange)){

            #ifdef TIMEZONE_API
                http.begin(String(TIMEZONE_API_URL) + "?key=" + TIMEZONE_API_KEY + "&format=json&fields=gmtOffset,zoneEnd&by=zone&zone=" + String(m_zones[i].zone.c_str()));
            #else
                http.begin(String(TIMEZONE_API_URL) + "?timeZone=" + String(m_zones[i].zone.c_str()));
            #endif
            int httpCode = http.GET();

            if (httpCode > 0) {
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, http.getString());
                if (!error) {
                    #ifdef TIMEZONE_API
                        m_zones[i].utcOffset = doc["gmtOffset"].as<int>();
                        if (doc["zoneEnd"].isNull()) {
                            // Timezone does not use DST, no futher updates necessary
                            m_zones[i].m_nextTimeZoneChange = 0;
                        } else {
                            // Timezone uses DST, update when necessary
                            m_zones[i].m_nextTimeZoneChange = doc["zoneEnd"].as<unsigned long>() + random(5 * 60); // Randomize update by 5 minutes to avoid flooding the API
                        }
                    #else
                        m_zones[i].utcOffset = doc["currentUtcOffset"]["seconds"].as<int>();
                        if (doc["hasDayLightSaving"].as<bool>()) {
                            m_zones[i].dstStart = doc["dstInterval"]["dstStart"].as<String>();
                            m_zones[i].dstEnd = doc["dstInterval"]["dstEnd"].as<String>();
                            m_zones[i].dstActive = doc["isDayLightSavingActive"].as<bool>();   
                            if (m_zones[i].dstActive) {
                                m_temp_t.Year = m_zones[i].dstEnd.substring(0,4).toInt() - 1970;
                                m_temp_t.Month = m_zones[i].dstEnd.substring(5,7).toInt();
                                m_temp_t.Day = m_zones[i].dstEnd.substring(8,10).toInt();
                                m_temp_t.Hour = m_zones[i].dstEnd.substring(11,13).toInt();
                                m_temp_t.Minute = m_zones[i].dstEnd.substring(14,16).toInt();
                                m_temp_t.Second = m_zones[i].dstEnd.substring(17,19).toInt();
                            } else {
                                m_temp_t.Year = m_zones[i].dstStart.substring(0,4).toInt() - 1970;
                                m_temp_t.Month = m_zones[i].dstStart.substring(5,7).toInt();
                                m_temp_t.Day = m_zones[i].dstStart.substring(8,10).toInt();
                                m_temp_t.Hour = m_zones[i].dstStart.substring(11,13).toInt();
                                m_temp_t.Minute = m_zones[i].dstStart.substring(14,16).toInt();
                                m_temp_t.Second = m_zones[i].dstStart.substring(17,19).toInt();
                            }
                            m_zones[i].m_nextTimeZoneChange = makeTime(m_temp_t);
                        }   
                        m_zones[i].tzLoaded = true;
                    #endif
                    Serial.print("Timezone Offset from API: ");
                    Serial.println(m_zones[0].utcOffset);
                    Serial.print("Next timezone update: ");
                    Serial.println(m_zones[0].m_nextTimeZoneChange);
                    // m_timeClient->setTimeOffset(m_timeZoneOffset);
                } else {
                    Serial.println("Deserialization error on timezone offset API response");
                }
            } else {
                Serial.println("Failed to get timezone offset from API");
            }
        }
        }
    }
}

bool GlobalTime::getFormat24Hour() {
    return m_format24hour;
}

bool GlobalTime::setFormat24Hour(bool format24hour) {
    m_format24hour = format24hour;
    return m_format24hour;
}