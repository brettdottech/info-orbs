#include "5ZoneWidget.h"
#include "5ZoneTranslations.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>

FiveZoneWidget::FiveZoneWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config) {
    m_enabled = (INCLUDE_5ZONE == WIDGET_ON);
    m_time = GlobalTime::getInstance();

    m_config.addConfigBool("FiveZoneWidget", "5zoEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigBool("FiveZoneWidget", "showBizHours", &m_showBizHours, t_5zoneShowBizHours, false);

    for (int i = 0; i < MAX_ZONES; i++) {
        const char *zoneName = strdup((String("5zoZoneName") + String(i)).c_str());
        const char *zoneDesc = strdup((i18nStr(t_5zoneDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigString("FiveZoneWidget", zoneName, &m_timeZones[i].locName, 50, zoneDesc, false);

        const char *zoneTZ = strdup((String("5zoZoneInfo") + String(i)).c_str());
        const char *zoneTZDesc = strdup((i18nStr(t_5zoneTZDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigString("FiveZoneWidget", zoneTZ, &m_timeZones[i].tzInfo, 50, zoneTZDesc, false);
    }

    for (int i = 0; i < MAX_ZONES; i++) {
        const char *zoneWorkStart = strdup((String("5zoZoneWstart") + String(i)).c_str());
        const char *zoneWorkStartDesc = strdup((i18nStr(t_5zoneWorkStartDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigInt("FiveZoneWidget", zoneWorkStart, &m_timeZones[i].m_workStart, zoneWorkStartDesc, true);

        const char *zoneWorkEnd = strdup((String("5zoZoneWend") + String(i)).c_str());
        const char *zoneWorkEndDesc = strdup((i18nStr(t_5zoneWorkEndDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigInt("FiveZoneWidget", zoneWorkEnd, &m_timeZones[i].m_workEnd, zoneWorkEndDesc, true);
    }
    m_format = m_config.getConfigInt("clockFormat", 0);
}

void FiveZoneWidget::setup() {
}

void FiveZoneWidget::getTZoneOffset(int8_t zoneIndex) {

    TimeZone &zone = m_timeZones[zoneIndex];
    HTTPClient http;
    http.begin(String(TIMEZONE_API_URL) + "?timeZone=" + String(zone.tzInfo.c_str()));
    http.setTimeout(5000); // Set 5 second timeout to prevent hanging

    Serial.println(String(TIMEZONE_API_URL) + "?timeZone=" + String(zone.tzInfo.c_str()));

    int httpCode = http.GET();
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (!error) {
            zone.timeZoneOffset = doc["currentUtcOffset"]["seconds"].as<int>();
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
                zone.nextTimeZoneUpdate = makeTime(m_temp_t) + random(5 * 60); // Randomize update by 5 minutes to avoid flooding the API;
            }
        } else {
            Log.warningln("Deserialization error on timezone offset API response");
        }
    } else {
        Log.warningln("Failed to get timezone offset from API, HTTP code: %d", httpCode);
    }
    http.end(); // Always close the connection
}

void FiveZoneWidget::update(bool force) {
    m_time->updateTime(true);
    int clockStamp = getClockStamp();

    if (clockStamp != m_clockStampU || force) {
        m_clockStampU = clockStamp;
        time_t lv_localEpoch = m_time->getUnixEpoch();

        for (int i = 0; i < MAX_ZONES; i++) {
            TimeZone &zone = m_timeZones[i];
            if (zone.timeZoneOffset == -1 || (zone.nextTimeZoneUpdate > 0 && lv_localEpoch > zone.nextTimeZoneUpdate)) {
                getTZoneOffset(i);
                delay(100);
            }
        }
    }
}

void FiveZoneWidget::changeFormat() {
    GlobalTime *time = GlobalTime::getInstance();
    m_format++;
    if (m_format > 1)
        m_format = 0;
    m_manager.clearAllScreens();
    update(true);
    draw(true);
}

int FiveZoneWidget::getClockStamp() {
    return m_time->getHour24() * 100 + m_time->getMinute();
}

void FiveZoneWidget::draw(bool force) {
    int clockStamp = getClockStamp();

    if (clockStamp != m_clockStampD || force) {
        m_clockStampD = clockStamp;
        for (int i = 0; i < MAX_ZONES; i++) {
            displayZone(i, force);
        }
    }
}

void FiveZoneWidget::displayZone(int8_t displayIndex, bool force) {
    const int nameY = 50; // Zone name at top
    const int dateY = 75; // Date indicator below name
    const int clockY = 115; // Time in middle
    const int ampmY = 175; // AM/PM indicator
    const int offsetY = 200; // Offset at bottom
    String lv_displayHour = "";
    String lv_offsetStr = " ";
    int lv_ringColor;
    String lv_dateIndicator = "";
    String lv_displayAM = "";
    time_t lv_unixEpoch;
    int lv_localDay;
    int lv_zoneDiff;
    int lv_hour;
    int lv_minute;
    int lv_day;
    int lv_weekday;
    int lv_hourD;
    int lv_minuteD;

    m_manager.setFont(DEFAULT_FONT);
    m_manager.selectScreen(displayIndex);

    if (force)
        m_manager.fillScreen(m_backgroundColor);
    m_foregroundColor = m_workColour;
    m_manager.setFontColor(m_foregroundColor);

    TimeZone &zone = m_timeZones[displayIndex];
    if (zone.locName != "") {
        // Get Orb (local) time information
        m_localTimeZone.locName = "Local Time";
        m_localTimeZone.timeZoneOffset = m_time->getTimeZoneOffset();
        m_unixEpoch = m_time->getUnixEpoch();

        // Get Time information for this TZ
        lv_unixEpoch = m_unixEpoch + zone.timeZoneOffset - m_localTimeZone.timeZoneOffset;
        lv_hour = hour(lv_unixEpoch);
        lv_minute = minute(lv_unixEpoch);
        lv_day = day(lv_unixEpoch);
        lv_weekday = weekday(lv_unixEpoch);

        // Calculate offset from local time
        lv_zoneDiff = zone.timeZoneOffset - m_localTimeZone.timeZoneOffset; // Difference between target UTC offset and local UTC offset
        lv_hourD = lv_zoneDiff / 3600;
        lv_minuteD = (lv_zoneDiff / 60) % 60;

        // calculate if day offset
        if (lv_zoneDiff > 0) {
            lv_offsetStr = "+";
            lv_ringColor = m_afterLocalTzColour;
        } else if (lv_zoneDiff < 0) {
            lv_offsetStr = "-";
            lv_hourD = lv_hourD * -1;
            lv_ringColor = m_beforeLocalTzColour;
        } else
            lv_ringColor = m_sameLocalTzColour;
        lv_offsetStr = lv_offsetStr + ((lv_hourD < 10) ? "0" : "") + String(lv_hourD) + ":" + ((lv_minuteD < 10) ? "0" : "") + String(lv_minuteD);

        // calculate if crossing date line
        lv_localDay = m_time->getDay();
        if (lv_localDay != lv_day) {
            if (lv_unixEpoch > m_unixEpoch)
                lv_dateIndicator = "+1d";
            else
                lv_dateIndicator = "-1d";
        }

        // 12/24 hour formate and AM/PM indicator
        if (m_format == 0) {
            lv_displayHour = ((lv_hour < 10) ? "0" : "") + String(lv_hour);
            lv_displayAM = "";
        } else {
            lv_displayHour = String(hourFormat12(lv_unixEpoch));
            lv_displayAM = (isAM(lv_unixEpoch)) ? "AM" : "PM";
        }

        m_manager.drawString(zone.locName.c_str(), ScreenCenterX, nameY, 18, Align::MiddleCenter);

        if (lv_dateIndicator != zone.m_lastDateIndicator || force) {
            m_manager.fillRect(ScreenCenterX - 80, ampmY - 10, 45, 22, m_backgroundColor);
            m_manager.drawString(lv_dateIndicator, ScreenCenterX - 60, ampmY, 16, Align::MiddleCenter);
            zone.m_lastDateIndicator = lv_dateIndicator;
        }

        if (lv_displayAM != zone.m_lastDisplayAM || force) {
            m_manager.fillRect(ScreenCenterX + 43, ampmY - 10, 37, 22, m_backgroundColor);
            m_manager.drawString(lv_displayAM, ScreenCenterX + 60, ampmY, 16, Align::MiddleCenter);
            zone.m_lastDisplayAM = lv_displayAM;
        }

        if (lv_zoneDiff != zone.m_zoneDiff || force) {
            m_manager.fillRect(ScreenCenterX - 35, offsetY - 10, 72, 22, m_backgroundColor);
            if (zone.timeZoneOffset == -1)
                m_manager.setFontColor(TFT_RED);
            m_manager.drawString(lv_offsetStr, ScreenCenterX, offsetY, 16, Align::MiddleCenter);
            m_manager.setFontColor(m_foregroundColor);
            zone.m_zoneDiff = lv_zoneDiff;
        }

        if (m_showBizHours) {
            m_manager.drawArc(120, 120, 120, 115, 0, 360, lv_ringColor, m_backgroundColor);
            if (isWeekend(lv_weekday)) {
                m_foregroundColor = m_weekendColor;
                m_manager.setFontColor(m_foregroundColor);
            } else {
                if (m_showBizHours) {
                    if (lv_hour < zone.m_workStart || lv_hour >= zone.m_workEnd) {
                        m_foregroundColor = m_afterWorkColour;
                        m_manager.setFontColor(m_foregroundColor);
                    }
                }
            }
        }

        String minuteStr = (lv_minute < 10) ? "0" + String(lv_minute) : String(lv_minute);
        String lv_displayTime = lv_displayHour + ":" + minuteStr;
        m_manager.fillRect(14, 82, 215, 69, m_backgroundColor);
        m_manager.drawString(lv_displayTime, ScreenCenterX, clockY, 62, Align::MiddleCenter);
    }
}

void FiveZoneWidget ::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        changeFormat();
    }
}

String FiveZoneWidget ::getName() {
    return "5 Zone Clock";
}
