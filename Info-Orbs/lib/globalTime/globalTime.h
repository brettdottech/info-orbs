#ifndef TIME_H
#define TIME_H

#include <NTPClient.h>
#include <WiFiUdp.h>


class Time {
public:
    Time();
    ~Time();

    void updateTime();
    static void getHourAndMinute(int &hour, int &minute);
    static void getUnixEpoch(time_t &epoch);
    static void getSecond(int &second);
    static void getDay(int &day);
    static void getMonth(int &month);
    static void getMonthName(String &monthName);
    static void getYear(int &year);
    static void getTime(String &time);
    static void getWeekday(String &weekday);

private:

    static time_t m_unixEpoch;
    static int m_hour;
    static int m_minute;
    static int m_second;
    static int m_day;
    static int m_month;
    static String m_monthName;
    static int m_year;
    static String m_time;
    static String m_weekday;

    WiFiUDP m_udp;
    NTPClient* m_timeClient{ nullptr };

    const int m_oneSecond{ 1000 };
    int m_updateTimer{ 0 };
};





#endif