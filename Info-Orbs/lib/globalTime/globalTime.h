#ifndef TIME_H
#define TIME_H

#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <config.h>

class GlobalTime {
   public:
    static GlobalTime *getInstance();

    void updateTime();
    void getHourAndMinute(int &hour, int &minute);
    int getHour();
    int getMinute();
    time_t getUnixEpoch();
    int getSecond();
    int getDay();
    int getMonth();
    String getMonthName();
    int getYear();
    String getTime();
    String getWeekday();
    void setTimeZoneOffset(int tz);
    bool isAM();

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
    int m_timeZoneOffset = TIME_ZONE_OFFSET;
    bool m_isAM = false;

    WiFiUDP m_udp;
    NTPClient *m_timeClient{nullptr};

    const int m_oneSecond{1000};
    int m_updateTimer{0};
};

#endif
