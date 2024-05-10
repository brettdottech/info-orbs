#ifndef TIME_H
#define TIME_H

#include <NTPClient.h>
#include <WiFiUdp.h>

class GlobalTime {
public:
  static GlobalTime* getInstance();

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

private:
  GlobalTime();
  ~GlobalTime();

  static GlobalTime *m_instance;

  time_t m_unixEpoch;
  int m_hour;
  int m_minute;
  int m_second;
  int m_day;
  int m_month;
  String m_monthName;
  int m_year;
  String m_time;
  String m_weekday;

  WiFiUDP m_udp;
  NTPClient *m_timeClient{nullptr};

  const int m_oneSecond{1000};
  int m_updateTimer{0};
};

#endif
