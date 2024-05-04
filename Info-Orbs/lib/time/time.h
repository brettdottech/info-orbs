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

private:

    static time_t m_unixEpoch;
    static int m_hour;
    static int m_minute;
    static int m_second;

    WiFiUDP m_udp;
    NTPClient* m_timeClient{ nullptr };

    const int m_oneSecond{ 1000 };
    int m_updateTimer{ 0 };
};





#endif