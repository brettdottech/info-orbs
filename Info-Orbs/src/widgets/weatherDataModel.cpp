#include "widgets/weatherDataModel.h"

WeatherDataModel::WeatherDataModel() {
}

WeatherDataModel &WeatherDataModel::setCityName(String city) {
    if (m_cityName != city) {
        m_cityName = city;
        m_changed = true;
    }
    return *this;
}

String WeatherDataModel::getCityName() {
    return m_cityName;
}

WeatherDataModel &WeatherDataModel::setCurrentText(String text) {
    if (m_currentWeatherText != text) {
        m_currentWeatherText = text;
        m_changed = true;
    }
    return *this;
}

String WeatherDataModel::getCurrentText() {
    return m_currentWeatherText;
}

WeatherDataModel &WeatherDataModel::setCurrentIcon(String icon) {
    if (m_currentWeatherIcon != icon) {
        m_currentWeatherIcon = icon;
        m_changed = true;
    }
    return *this;
}

String WeatherDataModel::getCurrentIcon() {
    return m_currentWeatherIcon;
}

WeatherDataModel &WeatherDataModel::setCurrentTemperature(float degrees) {
    if (m_currentWeatherDeg != degrees) {
        m_currentWeatherDeg = degrees;
        m_changed = true;
    }
    return *this;
}

float WeatherDataModel::getCurrentTemperature() {
    return m_currentWeatherDeg;
}

String WeatherDataModel::getCurrentTemperature(int8_t digits) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_currentWeatherDeg;
    return String(stream.str().c_str());
}

WeatherDataModel &WeatherDataModel::setTodayHigh(float high) {
    if (m_todayHigh != high) {
        m_todayHigh = high;
        m_changed = true;
    }
    return *this;
}

float WeatherDataModel::getTodayHigh() {
    return m_todayHigh;
}

String WeatherDataModel::getTodayHigh(int8_t digits) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_todayHigh;
    return String(stream.str().c_str());
}

WeatherDataModel &WeatherDataModel::setTodayLow(float low) {
    if (m_todayLow != low) {
        m_todayLow = low;
        m_changed = true;
    }
    return *this;
}

float WeatherDataModel::getTodayLow() {
    return m_todayLow;
}

String WeatherDataModel::getTodayLow(int8_t digits) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_todayLow;
    return String(stream.str().c_str());
}

WeatherDataModel &WeatherDataModel::setDaysIcons(String *icons) {
    for (int i; i < 3; i++) {
        setDayIcon(i, icons[i]);
    }
    return *this;
}

String &WeatherDataModel::getDaysIcons() {
    return *m_daysIcons;
}

WeatherDataModel &WeatherDataModel::setDayIcon(int num, String icon) {
    if (num < 3 && m_daysIcons[num] != icon) {
        m_daysIcons[num] = icon;
        m_changed = true;
    }
    return *this;
}

String WeatherDataModel::getDayIcon(int num) {
    if (num >= 3) {
        return "";
    }
    return m_daysIcons[num];
}

WeatherDataModel &WeatherDataModel::setDaysLows(float *lows) {
    for (int i; i < 3; i++) {
        setDayLow(i, lows[i]);
    }
    return *this;
}

WeatherDataModel &WeatherDataModel::setDayLow(int num, float low) {
    if (num < 3 && m_daysLow[num] != low) {
        m_daysLow[num] = low;
        m_changed = true;
    }
    return *this;
}

float &WeatherDataModel::getDaysLows() {
    return *m_daysLow;
}

float WeatherDataModel::getDayLow(int num) {
    if (num >= 3) {
        return NaN;
    }
    return m_daysLow[num];
}

String WeatherDataModel::getDayLow(int8_t num, int8_t digits) {
    if (m_daysHigh[num] == NaN) {
        return "";
    }
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_daysLow[num];
    return String(stream.str().c_str());
}

WeatherDataModel &WeatherDataModel::setDaysHighs(float highs[3]) {
    for (int i; i < 3; i++) {
        setDayHigh(i, highs[i]);
    }
    return *this;
}

float &WeatherDataModel::getDaysHighs() {
    return *m_daysHigh;
}

float WeatherDataModel::getDayHigh(int num) {
    if (num >= 3) {
        return NaN;
    }
    return m_daysHigh[num];
}

String WeatherDataModel::getDayHigh(int8_t num, int8_t digits) {
    if (m_daysHigh[num] == NaN) {
        return "";
    }
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_daysHigh[num];
    return String(stream.str().c_str());
}

WeatherDataModel &WeatherDataModel::setDayHigh(int num, float high) {
    if (num < 3 && m_daysHigh[num] != high) {
        if (m_daysHigh[num] != high) {
            m_daysHigh[num] = high;
            m_changed = true;
        }
    }
    return *this;
}

bool WeatherDataModel::isChanged() {
    return m_changed;
}
WeatherDataModel &WeatherDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}
