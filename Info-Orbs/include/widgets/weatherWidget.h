#ifndef WEAHTERWIDGET_H
#define WEAHTERWIDGET_H

#include <widget.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TJpg_Decoder.h>
#include "icons.h"

class WeatherWidget : public Widget {
public:
    WeatherWidget(ScreenManager& manager);
    ~WeatherWidget() override;
    void setup() override;
    void update() override;
    void draw() override;
private:

    void displayClock(int displayIndex, String time, String monthName, int day, String weekday, int color);

    void getWeatherData();
    void showJPG(int displayIndex, int x, int y, const byte a[], int z, int s);
    void drawWeatherIcon(String N, int displayIndex, int x, int y, int s);
    void singleWeatherDeg(int displayIndex, uint32_t b, uint32_t t);
    void weatherText(int displayIndex, int16_t b, int16_t t);
    void threeDayWeather(int displayIndex);
    void singleClockScreen(int displayIndex, uint32_t b, uint32_t t);

    String m_time;
    String m_lastTime;
    String m_monthName;
    String m_weekday;
    int m_day;

    const long m_weatherDelay = 600000; // weather refresh rate
    unsigned long m_weatherDelayPrev = 600000;
    const int centre = 120; // centre location of the screen(240x240)

    // Global Variables to track/store weather data
    String timeZoneOffSet;
    String cityName;
    String currentWeatherText; // Weather Description
    String currentWeatherIcon; // Text refrence for weather icon
    String currentWeatherDeg;
    String daysIcons[3]; // Icons/Temp For Next 3 days
    String daysDegs[3];
};


#endif // WEAHTERWIDGET_H