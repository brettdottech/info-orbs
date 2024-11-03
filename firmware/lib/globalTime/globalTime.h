#ifndef TIME_H
#define TIME_H

#include <NTPClient.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <config.h>

class GlobalTime {
   public:
    static GlobalTime *getInstance();

    void updateTime();
    void getHourAndMinute(int &hour, int &minute);
    int getHour();
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
    bool isPM();
    bool getFormat24Hour();
    bool setFormat24Hour(bool format24hour);

   private:
    GlobalTime();
    ~GlobalTime();

    static GlobalTime *m_instance;

    time_t m_unixEpoch;
    int m_hour = 0;
    int m_minute = 0;
    int m_second = 0;
    int m_day = 0;
    int m_month = 0;
    String m_monthName;
    int m_year = 0;
    String m_time;
    String m_weekday;
    int m_timeZoneOffset = -1; // A value that will be overwritten by the API

    WiFiUDP m_udp;
    NTPClient *m_timeClient{nullptr};

    unsigned long m_oneSecond = 1000;
    unsigned long m_updateTimer = 0;

    bool m_format24hour{FORMAT_24_HOUR};

    void getTimeZoneOffsetFromAPI();
};

#endif
