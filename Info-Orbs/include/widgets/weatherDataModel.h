#ifndef WEAHTERDATA_MODEL_H
#define WEAHTERDATA_MODEL_H

#include <Arduino.h>
#include <iomanip>
#include <sstream>

#define NaN -1024.0
class WeatherDataModel {
   public:
    WeatherDataModel();
    WeatherDataModel &setCityName(String city);
    String getCityName();
    WeatherDataModel &setCurrentText(String text);
    String getCurrentText();
    WeatherDataModel &setCurrentIcon(String icon);
    String getCurrentIcon();
    WeatherDataModel &setCurrentTemperature(float degrees);
    float getCurrentTemperature();
    String getCurrentTemperature(int8_t digits);
    WeatherDataModel &setTodayHigh(float high);
    float getTodayHigh();
    String getTodayHigh(int8_t digits);
    WeatherDataModel &setTodayLow(float low);
    float getTodayLow();
    String getTodayLow(int8_t digits);

    WeatherDataModel &setDaysIcons(String icons[3]);
    String &getDaysIcons();
    WeatherDataModel &setDayIcon(int num, String icon);
    String getDayIcon(int num);

    WeatherDataModel &setDaysHighs(float highs[3]);
    float &getDaysHighs();
    WeatherDataModel &setDayHigh(int num, float high);
    float getDayHigh(int num);
    String getDayHigh(int8_t num, int8_t digits);

    WeatherDataModel &setDaysLows(float lows[3]);
    float &getDaysLows();
    WeatherDataModel &setDayLow(int num, float low);
    float getDayLow(int num);
    String getDayLow(int8_t num, int8_t digits);

    bool isChanged();
    WeatherDataModel &setChangedStatus(bool changed);

   private:
    String m_cityName;
    String m_currentWeatherText = "";  // Weather Description
    String m_currentWeatherIcon = "";  // Text refrence for weather icon
    float m_currentWeatherDeg = 0.0;
    float m_todayHigh = 0.0;
    float m_todayLow = 0.0;

    String m_daysIcons[3] = { "", "", "" };
    float m_daysHigh[3] = { NaN, NaN, NaN };
    float m_daysLow[3] = { NaN, NaN, NaN };

    bool m_changed = false;
};
#endif  // WEAHTER_DATA_MODEL_H