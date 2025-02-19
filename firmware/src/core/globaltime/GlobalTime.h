#ifndef GLOBALTIME_H
#define GLOBALTIME_H

#include "config_helper.h"
#include <HTTPClient.h>
#include <NTPClient.h>
#include <TimeLib.h>

// Define locales
#define EN 0
#define DE 1
#define FR 2

#if LOCALE == DE // German
const char LOC_MONTH[12][10] = {"Januar", "Februar", "Maerz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"}; // Define german for month
const char LOC_WEEKDAY[7][11] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"}; // Define german for weekday
const String LOC_FORMAT_DAYMONTH = "%d. %B"; // in strftime format
const String LOC_LANG = "de";
#elif LOCALE == FR // French
const char LOC_MONTH[12][10] = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"}; // Define french for month
const char LOC_WEEKDAY[7][11] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"}; // Define french for weekday
const String LOC_FORMAT_DAYMONTH = "%d %B"; // in strftime format
const String LOC_LANG = "fr";
#else // English
const char LOC_MONTH[12][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}; // Define english for month
const char LOC_WEEKDAY[7][11] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Define english for weekday
const String LOC_FORMAT_DAYMONTH = "%d %B"; // in strftime format, this will be overriden if WEATHER_UNITS_METRIC is not set
const String LOC_LANG = "en";
#endif

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

struct TimeZone {
    std::string zone = "";
    std::string cityName = "";      // Display name of the city
    int dst = 0;
    int utcOffset = 0;       // UTC offset in hours
    int offsetToLocal = 0;
    String dstStart = "";
    String dstEnd = "";
    bool dstActive = false;
    bool tzLoaded = false;
    int m_nextTimeZoneChange = 0;
};

class GlobalTime {
public:
    static GlobalTime *getInstance();

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
    void setTZforTime(int &offSet);
    int getUTCoffset(int &zoneId);
    String getZoneName(int &zoneId);

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
    
    TimeZone m_zones[MAX_ZONES];
    tmElements_t m_temp_t;
    String m_dstStart;
    String m_dstEnd;
    bool m_dstActive;

    int m_timeZoneOffset = -1; // A value that will be overwritten by the API
    unsigned long m_nextTimeZoneUpdate = 0;

    std::string m_zone0{TIMEZONE_API_LOCATION};
    std::string m_cityName0{TIMEZONE_API_NAME};
    std::string m_zone1{TIMEZONE_API_LOCATION1};
    std::string m_cityName1{TIMEZONE_API_NAME1};
    std::string m_zone2{TIMEZONE_API_LOCATION2};
    std::string m_cityName2{TIMEZONE_API_NAME2};
    std::string m_zone3{TIMEZONE_API_LOCATION3};
    std::string m_cityName3{TIMEZONE_API_NAME3};
    std::string m_zone4{TIMEZONE_API_LOCATION4};
    std::string m_cityName4{TIMEZONE_API_NAME4};

    WiFiUDP m_udp;
    NTPClient *m_timeClient{nullptr};

    unsigned long m_oneSecond = 1000;
    unsigned long m_updateTimer = 0;

    bool m_format24hour{FORMAT_24_HOUR};
    std::string m_ntpServer{NTP_SERVER};

    void getTimeZoneOffsetFromAPI();
};

#endif // GLOBALTIME_H