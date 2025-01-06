#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include "GlobalTime.h"
#include "WeatherDataModel.h"
#include "Widget.h"
#include "config_helper.h"
#include <TaskManager.h>

class WeatherWidget : public Widget {
public:
    WeatherWidget(ScreenManager &manager, ConfigManager &config);
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
    void drawWeatherIcon(int displayIndex, const String &condition, int x, int y, int scale);
    void singleWeatherDeg(int displayIndex);
    void weatherText(int displayIndex);
    void threeDayWeather(int displayIndex);
    bool getWeatherData();
    int getClockStamp();
    void configureColors();
    void preProcessResponse(int httpCode, String& response);
    void processResponse(int httpCode, const String& response);

    GlobalTime *m_time;
    int8_t m_mode;

#ifdef WEATHER_SCREEN_MODE
    int m_screenMode = WEATHER_SCREEN_MODE;
#else
    int m_screenMode = Dark;
#endif
    uint16_t m_foregroundColor;
    uint16_t m_backgroundColor;
    uint16_t m_invertedForegroundColor;
    uint16_t m_invertedBackgroundColor;

    const long m_weatherDelay = 600000; // Weather refresh rate
    unsigned long m_weatherDelayPrev = 0;

    const int centre = 120; // Centre location of the screen(240x240)

    int m_clockStamp = 0;

    WeatherDataModel model;

// This is a hack to support old config.h files that have WEATHER_LOCAION instead of LOCATION.
#ifndef WEATHER_LOCATION
    #define WEATHER_LOCATION WEATHER_LOCAION
#endif

    std::string m_weatherLocation = WEATHER_LOCATION;

#ifdef WEATHER_UNITS_METRIC
    int m_weatherUnits = 0;
#else
    int m_weatherUnits = 1;
#endif

    const String weatherApiKey = WEATHER_API_KEY;

    const int MODE_HIGHS = 0;
    const int MODE_LOWS = 1;
};
#endif // WEATHERWIDGET_H
