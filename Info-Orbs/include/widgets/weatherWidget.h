#ifndef WEAHTERWIDGET_H
#define WEAHTERWIDGET_H

#include <widget.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TJpg_Decoder.h>
#include "icons.h"
#include <config.h>

class WeatherWidget : public Widget {
public:
    WeatherWidget(ScreenManager& manager);
    ~WeatherWidget() override;
    void setup() override;
    void update() override;
    void draw() override;
private:

    void displayClock(int displayIndex, int color);

    void showJPG(int displayIndex, int x, int y, const byte jpgData[], int size, int scale);
    void drawWeatherIcon(String condition, int displayIndex, int x, int y, int scale);
    void singleWeatherDeg(int displayIndex, uint32_t background, uint32_t textColor);
    void weatherText(int displayIndex, int16_t background, int16_t textColor);
    void threeDayWeather(int displayIndex);
    String getClockStamp();
    String getWeatherStamp();

    String m_hour;
    String m_minute;
    String m_month;
    String m_monthName;
    String m_weekday;
    String m_day;


    const long m_weatherDelay = 600000; // weather refresh rate
    unsigned long m_lastWeather = 600000;

    const long m_timeDelay = 1000; // time refresh rate
    unsigned long m_lastTime;

    const int centre = 120; // centre location of the screen(240x240)

    String m_clockStamp = "";
    String m_weatherStamp = "";

    // Global Variables to track/store weather data
    int timeZoneOffSet;
    String cityName;
    String currentWeatherText; // Weather Description
    String currentWeatherIcon; // Text refrence for weather icon
    String currentWeatherDeg;
    String daysIcons[3]; // Icons/Temp For Next 3 days
    String daysDegs[3];

    String weatherLocation = WEATHER_LOCAION;
    String weatherUnits = WEATHER_UNITS;
    String weatherApiKey = WEATHER_API_KEY;

    String httpRequestAddress = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                                weatherLocation + "/next3days?key=" + weatherApiKey + "&unitGroup=" + weatherUnits +
                                "&include=days,current&iconSet=icons1";

};


#endif // WEAHTERWIDGET_H