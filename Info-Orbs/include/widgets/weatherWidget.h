#ifndef WEAHTERWIDGET_H
#define WEAHTERWIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>
#include <config.h>
#include <globalTime.h>
#include <math.h>
#include <widget.h>

#include "icons.h"
#include "weatherDataModel.h"

class WeatherWidget : public Widget {
   public:
    WeatherWidget(ScreenManager& manager);
    ~WeatherWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void changeMode() override;

   private:
    void displayClock(int displayIndex, uint32_t background, uint32_t textColor);

    void showJPG(int displayIndex, int x, int y, const byte jpgData[], int size, int scale);
    void drawWeatherIcon(String condition, int displayIndex, int x, int y, int scale);
    void singleWeatherDeg(int displayIndex, uint32_t background, uint32_t textColor);
    void weatherText(int displayIndex, int16_t background, int16_t textColor);
    void threeDayWeather(int displayIndex);
    bool getWeatherData();
    int getClockStamp();
    int drawDegrees(String number, int x, int y, uint8_t font, uint8_t size, uint8_t outerRadius, uint8_t innerRadius, int16_t textColor, int16_t backgroundColor);

    GlobalTime* m_time;
    int8_t m_mode;

    const long m_updateDelay = 600000;  // weather refresh rate
    unsigned long m_lastUpdate = 0;

    const long m_timeDelay = 1000;  // time refresh rate
    unsigned long m_lastTime;

    const int centre = 120;  // centre location of the screen(240x240)

    int m_clockStamp = 0;

    WeatherDataModel model;

    String weatherLocation = WEATHER_LOCAION;
#ifdef WEATHER_UNITS_METRIC
    String weatherUnits = "metric";
#else
    String weatherUnits = "us";
#endif
    String weatherApiKey = WEATHER_API_KEY;

    String httpRequestAddress = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                                weatherLocation + "/next3days?key=" + weatherApiKey + "&unitGroup=" + weatherUnits +
                                "&include=days,current&iconSet=icons1";

    const int MODE_HIGHS = 0;
    const int MODE_LOWS = 1;
};
#endif  // WEAHTERWIDGET_H