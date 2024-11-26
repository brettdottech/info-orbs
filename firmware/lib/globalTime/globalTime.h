#ifndef TIME_H
#define TIME_H

#include <NTPClient.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <config_helper.h>

// Define locales
#define EN 0
#define DE 1
#define FR 2

#if LOCALE==DE  // German
   const char LOC_MONTH[12][10] = {"Januar","Februar","Maerz","April","Mai","Juni","Juli","August","September","Oktober","November","Dezember"}; // Define german for month
   const char LOC_WEEKDAY[7][11] = {"Sonntag","Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag"}; // Define german for weekday
   const String LOC_FORMAT_DAYMONTH = "%d. %B"; // in strftime format
   const String LOC_LANG = "de";
#elif LOCALE==FR // French
    const char LOC_MONTH[12][10] = {"Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"}; // Define french for month
    const char LOC_WEEKDAY[7][11] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"}; // Define french for weekday
    const String LOC_FORMAT_DAYMONTH ="%d %B"; // in strftime format
    const String LOC_LANG = "fr";
#else // English
    const char LOC_MONTH[12][10] = {"January","February","March","April","May","June","July","August","September","October","November","December"}; // Define english for month
    const char LOC_WEEKDAY[7][11] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"}; // Define english for weekday
    const String LOC_FORMAT_DAYMONTH = "%d %B"; // in strftime format, this will be overriden if WEATHER_UNITS_METRIC is not set
    const String LOC_LANG = "en";
#endif

class GlobalTime {
   public:
    static GlobalTime *getInstance();

    void updateTime();
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

   private:
    GlobalTime();
    ~GlobalTime();

    static GlobalTime *m_instance;

    time_t m_unixEpoch;
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
    int m_timeZoneOffset = -1; // A value that will be overwritten by the API
    unsigned long m_nextTimeZoneUpdate = 0;

    WiFiUDP m_udp;
    NTPClient *m_timeClient{nullptr};

    unsigned long m_oneSecond = 1000;
    unsigned long m_updateTimer = 0;

    bool m_format24hour{FORMAT_24_HOUR};

    void getTimeZoneOffsetFromAPI();
};

#endif
