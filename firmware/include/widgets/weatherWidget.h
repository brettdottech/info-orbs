#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>
#include <config_helper.h>
#include <globalTime.h>
#include <math.h>
#include <widget.h>
#include <utils.h>
#include "model/weatherDataModel.h"

class WeatherWidget : public Widget {
   public:
    WeatherWidget(ScreenManager& manager);
    ~WeatherWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;
private:
    void displayClock(int displayIndex);
    void changeMode();
    void displayClock(int displayIndex, uint32_t background, uint32_t textColor);
    void showJPG(int displayIndex, int x, int y, const byte jpgData[], int size, int scale);
    void drawWeatherIcon(int displayIndex, const String& condition, int x, int y, int scale);
    void singleWeatherDeg(int displayIndex);
    void weatherText(int displayIndex);
    void threeDayWeather(int displayIndex);
    bool getWeatherData();
    int getClockStamp();
    void configureColors();

    GlobalTime* m_time;
    int8_t m_mode;

    ScreenMode m_screenMode = Dark;
    uint16_t m_foregroundColor;
    uint16_t m_backgroundColor;
    uint16_t m_invertedForegroundColor;
    uint16_t m_invertedBackgroundColor;

    const long m_weatherDelay = 600000;  // Weather refresh rate
    unsigned long m_weatherDelayPrev = 0;

    const int centre = 120;  // Centre location of the screen(240x240)

    int m_clockStamp = 0;

    WeatherDataModel model;

// This is a hack to support old config.h files that have WEATHER_LOCAION instead of LOCATION.
#ifndef WEATHER_LOCATION
    #define WEATHER_LOCATION WEATHER_LOCAION
#endif

    const String weatherLocation = WEATHER_LOCATION;
#ifdef WEATHER_UNITS_METRIC
    const String weatherUnits = "metric";
#else
    const String weatherUnits = "us";
#endif
    const String weatherApiKey = WEATHER_API_KEY;

    const String httpRequestAddress = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                                      weatherLocation + "/next3days?key=" + weatherApiKey + "&unitGroup=" + weatherUnits +
                                      "&include=days,current&iconSet=icons1&lang=" + LOC_LANG;

    const int MODE_HIGHS = 0;
    const int MODE_LOWS = 1;
};
#endif  // WEATHERWIDGET_H
