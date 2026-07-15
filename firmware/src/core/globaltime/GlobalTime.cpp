#include "GlobalTime.h"

#include "ConfigManager.h"
#include "Translations.h"
#include "config_helper.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>

GlobalTime *GlobalTime::m_instance = nullptr;

GlobalTime::GlobalTime() {
    ConfigManager *cm = ConfigManager::getInstance();
    m_timezoneLocation = cm->getConfigString("timezoneLoc", m_timezoneLocation); // config added in MainHelper
    int clockFormat = cm->getConfigInt("clockFormat", CLOCK_FORMAT); // config added in ClockWidget
    m_ntpServer = cm->getConfigString("ntpServer", m_ntpServer); // config added in MainHelper
    Log.infoln("GlobalTime initialized, tzLoc=%s, clockFormat=%d, ntpServer=%s", m_timezoneLocation.c_str(), clockFormat, m_ntpServer.c_str());
    m_format24hour = (clockFormat == CLOCK_FORMAT_24_HOUR);
    m_timeClient = new NTPClient(m_udp, m_ntpServer.c_str(), 0, m_updateInterval);
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

time_t GlobalTime::getUnixEpochIfAvailable() {
    return m_instance ? m_instance->getUnixEpoch() : 0;
}

void GlobalTime::updateTime(bool force) {
    // Rollover-safe, drift-free 1s scheduler.
    // Advancing m_updateTimer by whole periods (instead of setting it to now)
    // keeps this sampler phase-locked to the 1000ms grid of millis(), so
    // displayed seconds are never skipped due to accumulated drift (the old
    // "> 1000 then timer = millis()" version drifted by the loop latency on
    // every tick and skipped roughly one displayed second per 3 minutes).
    uint32_t now = millis();
    uint32_t elapsed = now - (uint32_t) m_updateTimer;
    if (!force && elapsed < m_oneSecond) {
        return;
    }
    if (elapsed >= m_oneSecond) {
        m_updateTimer += m_oneSecond * (elapsed / m_oneSecond);
    }
    // NTPClient::update() re-attempts a *blocking* forceUpdate() (up to
    // ~1010ms of delay(10) waiting for the UDP reply) on EVERY call once a
    // refresh is due, because a failed attempt never advances _lastUpdate.
    // If the NTP server stops responding, that turns this once-per-second
    // tick into a ~1s stall of the whole render loop. Gate the attempts
    // ourselves so a dead server costs at most one stall per retry
    // interval; timekeeping continues via millis() inside getEpochTime().
    uint32_t ntpRetryInterval = m_timeClient->isTimeSet() ? m_ntpRetryInterval : m_ntpInitialRetryInterval;
    if (force || m_lastNtpAttempt == 0 || (uint32_t) (now - m_lastNtpAttempt) >= ntpRetryInterval) {
        m_lastNtpAttempt = now;
        if (m_timeClient->update()) {
            m_lastNtpSync = now;
        } else if (m_timeClient->isTimeSet() && (uint32_t) (now - m_lastNtpSync) >= m_updateInterval + m_ntpRetryInterval) {
            // Only a real failure: a refresh is overdue (m_updateInterval
            // has passed since the last successful sync) and the attempt
            // did not succeed. update() also returns false when a refresh
            // simply is not due yet, which must not be logged.
            Log.warningln("NTP update failed (server not responding), will retry later");
        }
    }
    if (m_timeClient->isTimeSet()) {
        // NTP time is valid
        if (m_timeZoneOffset == -1 || (m_nextTimeZoneUpdate > 0 && m_unixEpoch > m_nextTimeZoneUpdate)) {
            // getTimeZoneOffsetFromAPI() is a blocking HTTP call (multiple
            // seconds on an unreachable server). Without gating it is
            // retried on every 1s tick until it succeeds (boot with a dead
            // API, or DST changeover), stalling the render loop each time.
            uint32_t tzRetryInterval = (m_timeZoneOffset == -1) ? m_tzInitialRetryInterval : m_tzRetryInterval;
            if (m_lastTimeZoneAttempt == 0 || (uint32_t) (now - m_lastTimeZoneAttempt) >= tzRetryInterval) {
                m_lastTimeZoneAttempt = now;
                getTimeZoneOffsetFromAPI();
            }
        }
        m_unixEpoch = m_timeClient->getEpochTime();
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
        m_monthName = i18n(t_months, m_month - 1);
        m_year = year(m_unixEpoch);
        m_weekday = i18n(t_weekdays, weekday(m_unixEpoch) - 1);
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
    String retVal = i18n(t_dayMonthFormat);
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

void GlobalTime::getTimeZoneOffsetFromAPI() {
    HTTPClient http;
    http.begin(String(TIMEZONE_API_URL) + "?timeZone=" + String(m_timezoneLocation.c_str()));

    int httpCode = http.GET();

    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (!error) {
            m_timeZoneOffset = doc["currentUtcOffset"]["seconds"].as<int>();
            if (doc["hasDayLightSaving"].as<bool>()) {
                String dstStart = doc["dstInterval"]["dstStart"].as<String>();
                String dstEnd = doc["dstInterval"]["dstEnd"].as<String>();
                bool dstActive = doc["isDayLightSavingActive"].as<bool>();
                tmElements_t m_temp_t;
                if (dstActive) {
                    m_temp_t.Year = dstEnd.substring(0, 4).toInt() - 1970;
                    m_temp_t.Month = dstEnd.substring(5, 7).toInt();
                    m_temp_t.Day = dstEnd.substring(8, 10).toInt();
                    m_temp_t.Hour = dstEnd.substring(11, 13).toInt();
                    m_temp_t.Minute = dstEnd.substring(14, 16).toInt();
                    m_temp_t.Second = dstEnd.substring(17, 19).toInt();
                } else {
                    m_temp_t.Year = dstStart.substring(0, 4).toInt() - 1970;
                    m_temp_t.Month = dstStart.substring(5, 7).toInt();
                    m_temp_t.Day = dstStart.substring(8, 10).toInt();
                    m_temp_t.Hour = dstStart.substring(11, 13).toInt();
                    m_temp_t.Minute = dstStart.substring(14, 16).toInt();
                    m_temp_t.Second = dstStart.substring(17, 19).toInt();
                }
                m_nextTimeZoneUpdate = makeTime(m_temp_t) + random(5 * 60); // Randomize update by 5 minutes to avoid flooding the API;
            }
            Log.infoln("Timezone Offset from API: %d; Next timezone update: %d", m_timeZoneOffset, m_nextTimeZoneUpdate);
            m_timeClient->setTimeOffset(m_timeZoneOffset);
        } else {
            Log.warningln("Deserialization error on timezone offset API response");
        }
    } else {
        Log.warningln("Failed to get timezone offset from API");
    }
}

bool GlobalTime::getFormat24Hour() {
    return m_format24hour;
}

bool GlobalTime::setFormat24Hour(bool format24hour) {
    m_format24hour = format24hour;
    return m_format24hour;
}

int GlobalTime::getTimeZoneOffset() {
    return m_timeZoneOffset;
}
