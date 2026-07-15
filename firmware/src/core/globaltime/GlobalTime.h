#ifndef GLOBALTIME_H
#define GLOBALTIME_H

#include "config_helper.h"
#include <HTTPClient.h>
#include <NTPClient.h>
#include <TimeLib.h>

enum ClockFormat {
    CLOCK_FORMAT_24_HOUR = 0,
    CLOCK_FORMAT_12_HOUR = 1,
    CLOCK_FORMAT_12_HOUR_AMPM = 2
};

#if FORMAT_24_HOUR == true
    #define CLOCK_FORMAT CLOCK_FORMAT_24_HOUR
#else
    #if SHOW_AM_PM_INDICATOR == false
        #define CLOCK_FORMAT CLOCK_FORMAT_12_HOUR
    #else
        #define CLOCK_FORMAT CLOCK_FORMAT_12_HOUR_AMPM
    #endif
#endif

class GlobalTime {
public:
    static GlobalTime *getInstance();
    static time_t getUnixEpochIfAvailable();

    void updateTime(bool force = false);
    void getHourAndMinute(int &hour, int &minute);
    int getHour();
    int getHour24();
    String getHourPadded();
    int getMinute();
    String getMinutePadded();
    time_t getUnixEpoch();
    int getSecond();
    int getDay();
    int getMonth();
    String getMonthName();
    int getYear();
    String getTime();
    String getWeekday();
    String getDayAndMonth();
    bool isPM();
    bool getFormat24Hour();
    bool setFormat24Hour(bool format24hour);
    int getTimeZoneOffset();

private:
    GlobalTime();
    ~GlobalTime();

    static GlobalTime *m_instance;

    time_t m_unixEpoch = 0;
    int m_hour = 0;
    int m_hour24 = 0;
    int m_minute = 0;
    int m_second = 0;
    int m_day = 0;
    int m_month = 0;
    String m_monthName;
    int m_year = 0;
    String m_time;
    String m_weekday;
    std::string m_timezoneLocation = TIMEZONE_API_LOCATION;
    int m_timeZoneOffset = -1; // A value that will be overwritten by the API
    unsigned long m_nextTimeZoneUpdate = 0;

    WiFiUDP m_udp;
    NTPClient *m_timeClient{nullptr};

    unsigned long m_updateInterval = 900000; // Update every 15 min
    const int m_lowYearTest = 2025;
    const int m_highYearTest = 2035;
    unsigned long m_oneSecond = 1000;
    unsigned long m_updateTimer = 0;

    // NTP attempt gating (see updateTime()): NTPClient's blocking forceUpdate()
    // is re-attempted on every update() call once a refresh is due, so failed
    // attempts must be rate-limited ourselves.
    unsigned long m_ntpRetryInterval = 61 * 1000; // between attempts after a failed refresh
    unsigned long m_ntpInitialRetryInterval = 5 * 1000; // until the first successful sync after boot
    unsigned long m_lastNtpAttempt = 0;
    unsigned long m_lastNtpSync = 0;

    // Timezone API attempt gating (see updateTime()): the API call is a
    // blocking HTTP request and must not be retried on every 1s tick.
    unsigned long m_tzInitialRetryInterval = 30 * 1000; // until the first successful fetch
    unsigned long m_tzRetryInterval = 5 * 60 * 1000; // after a failed refresh
    unsigned long m_lastTimeZoneAttempt = 0;

    bool m_format24hour{FORMAT_24_HOUR};
    std::string m_ntpServer{NTP_SERVER};

    void getTimeZoneOffsetFromAPI();
};

#endif // GLOBALTIME_H
