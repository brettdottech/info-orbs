// TODO:
// 1
// factor out a selectDisplay that selects the dispay and returns the workable object,
// so we don't have this select, getDisplay crap all over the place
// 2
// make high/low an enum, if we even keep it (I strongly suggest just switching back
// and forth between high and low every 10 seconds or something)
// 3
// factor out the JSON error handling (come on now)
// 4
// factor out the text wrapping (there's a utils for that already, if that doesn't work, why not?)

#include "WeatherWidget.h"
#include "icons.h"

#include "config_helper.h"

WeatherWidget::WeatherWidget(ScreenManager &manager) : Widget(manager) {
    m_mode = MODE_HIGHS;
}

WeatherWidget::~WeatherWidget() {
}

void WeatherWidget::changeMode() {
    m_mode++;
    if (m_mode > MODE_LOWS) {
        m_mode = MODE_HIGHS;
    }
    threeDayWeather(4);
}

void WeatherWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        changeMode();
}

void WeatherWidget::setup() {
    m_time = GlobalTime::getInstance();
#ifdef WEATHER_SCREEN_MODE
    m_screenMode = WEATHER_SCREEN_MODE;
#endif
    configureColors();
}

void WeatherWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);
    m_time->updateTime();
    int clockStamp = getClockStamp();
    if (clockStamp != m_clockStamp || force) {
        displayClock(0);
        m_clockStamp = clockStamp;
    }

    if (force || model.isChanged()) {
        weatherText(1);
        drawWeatherIcon(2, model.getCurrentIcon(), 0, 0, 1);
        singleWeatherDeg(3);
        threeDayWeather(4);
        model.setChangedStatus(false);
    }
}

void WeatherWidget::update(bool force) {
    if (force || m_weatherDelayPrev == 0 || (millis() - m_weatherDelayPrev) >= m_weatherDelay) {
        setBusy(true);
        if (force) {
            int retry = 0;
            while (!getWeatherData() && retry++ < MAX_RETRIES)
                ;
        } else {
            getWeatherData();
        }
        setBusy(false);
        m_weatherDelayPrev = millis();
    }
}

bool WeatherWidget::getWeatherData() {
    HTTPClient http;
    http.begin(httpRequestAddress);
    int httpCode = http.GET();
    if (httpCode > 0) {
        // Check for the return code   TODO: factor out
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        http.end();

        if (!error) {
            model.setCityName(doc["resolvedAddress"].as<String>());
            model.setCurrentTemperature(doc["currentConditions"]["temp"].as<float>());
            model.setCurrentText(doc["days"][0]["description"].as<String>());

            model.setCurrentIcon(doc["currentConditions"]["icon"].as<String>());
            model.setTodayHigh(doc["days"][0]["tempmax"].as<float>());
            model.setTodayLow(doc["days"][0]["tempmin"].as<float>());
            for (int i = 0; i < 3; i++) {
                model.setDayIcon(i, doc["days"][i + 1]["icon"].as<String>());
                model.setDayHigh(i, doc["days"][i + 1]["tempmax"].as<float>());
                model.setDayLow(i, doc["days"][i + 1]["tempmin"].as<float>());
            }
        } else {
            // Handle JSON deserialization error
            switch (error.code()) {
            case DeserializationError::Ok:
                Serial.print(F("Deserialization succeeded"));
                break;
            case DeserializationError::InvalidInput:
                Serial.print(F("Invalid input!"));
                break;
            case DeserializationError::NoMemory:
                Serial.print(F("Not enough memory"));
                break;
            default:
                Serial.print(F("Deserialization failed"));
                break;
            }

            return false;
        }
    } else {
        // Handle HTTP request error
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }
    return true;
}

void WeatherWidget::displayClock(int displayIndex) {
    const int clockY = 120;
    const int dayOfWeekY = 190;
    const int dateY = 50;

    m_manager.selectScreen(displayIndex);
    m_manager.fillScreen(m_backgroundColor);
    m_manager.setFontColor(m_foregroundColor);

    m_manager.drawCentreString(m_time->getDayAndMonth(), centre, dateY, 18);
    const String weekDay = m_time->getWeekday();
    m_manager.drawCentreString(weekDay, centre, dayOfWeekY, 22);

    m_manager.drawString(m_time->getHourPadded(), centre - 10, clockY, 66, Align::MiddleRight);
    m_manager.drawString(":", centre, clockY, 66, Align::MiddleCenter);
    m_manager.drawString(m_time->getMinutePadded(), centre + 10, clockY, 66, Align::MiddleLeft);
}

// Write an image to the screen from a hex array.
// scale of the image (1=full size, then multiples of 2 to scale down)
// getting the byte array size is very annoying as it's computed on compile, so you can't do it dynamically.
void WeatherWidget::showJPG(int displayIndex, int x, int y, const byte jpgData[], int jpgDataSize, int scale) {
    m_manager.selectScreen(displayIndex);

    TJpgDec.setJpgScale(scale);
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, jpgData, jpgDataSize);
    TJpgDec.drawJpg(x, y, jpgData, jpgDataSize);
}

// Take the text output from the weather API and map it to a icon/byte array, then display it
void WeatherWidget::drawWeatherIcon(int displayIndex, const String &condition, int x, int y, int scale) {
    const byte *iconStart = NULL;
    const byte *iconEnd = NULL;

    if (condition == "partly-cloudy-night") {
        iconStart = m_screenMode == Light ? moonCloudW_start : moonCloudB_start;
        iconEnd = m_screenMode == Light ? moonCloudW_end : moonCloudB_end;
    } else if (condition == "partly-cloudy-day") {
        iconStart = m_screenMode == Light ? sunCloudsW_start : sunCloudsB_start;
        iconEnd = m_screenMode == Light ? sunCloudsW_end : sunCloudsB_end;
    } else if (condition == "clear-day") {
        iconStart = m_screenMode == Light ? sunW_start : sunB_start;
        iconEnd = m_screenMode == Light ? sunW_end : sunB_end;
    } else if (condition == "clear-night") {
        iconStart = m_screenMode == Light ? moonW_start : moonB_start;
        iconEnd = m_screenMode == Light ? moonW_end : moonB_end;
    } else if (condition == "snow") {
        iconStart = m_screenMode == Light ? snowW_start : snowB_start;
        iconEnd = m_screenMode == Light ? snowW_end : snowB_end;
    } else if (condition == "rain") {
        iconStart = m_screenMode == Light ? rainW_start : rainB_start;
        iconEnd = m_screenMode == Light ? rainW_end : rainB_end;
    } else if (condition == "fog" || condition == "wind" || condition == "cloudy") {
        iconStart = m_screenMode == Light ? cloudsW_start : cloudsB_start;
        iconEnd = m_screenMode == Light ? cloudsW_end : cloudsB_end;
    } else {
        Serial.println("unknown weather icon:" + condition);
    }

    const int size = iconEnd - iconStart;
    if (iconStart != NULL && size > 0) {
        showJPG(displayIndex, x, y, iconStart, size, scale);
    }
}

// Displays the current temperature on a single screen.
// doesn't round deg, just removes all text after the decimal
void WeatherWidget::singleWeatherDeg(int displayIndex) {
    m_manager.selectScreen(displayIndex);
    m_manager.fillScreen(m_backgroundColor);
    m_manager.drawCentreString(model.getCurrentTemperature(0), centre, 90, 88);

    // No glaring white chunks in Dark mode
    if (m_screenMode == Light) {
        m_manager.fillRect(0, 150, 240, 90, m_foregroundColor);
        m_manager.fillRect(centre - 1, 150, 2, 90, m_backgroundColor);
    }

    int fontSize = 22;
    m_manager.setFontColor(m_invertedForegroundColor);
    m_manager.setBackgroundColor(m_invertedBackgroundColor);
    m_manager.drawCentreString("High", 80, 170, fontSize);
    m_manager.drawCentreString("Low", 160, 170, fontSize);
    m_manager.drawCentreString(model.getTodayHigh(0), 80, 210, fontSize);
    m_manager.drawCentreString(model.getTodayLow(0), 160, 210, fontSize);
    m_manager.setFontColor(m_foregroundColor);
    m_manager.setBackgroundColor(m_backgroundColor);
}

// Display the user's current city and the text description of the weather
void WeatherWidget::weatherText(int displayIndex) {
    m_manager.selectScreen(displayIndex);

    //=== TEXT OVERFLOW ============================
    // This takes a given string a and breaks it down in max x character long strings ensuring not to break it only at a space.
    // Given the small width of the screens this will porbablly be needed to this project again so making sure to outline it
    // clearly as this should liekly eventually be turned into a fucntion. Before use the array size should be made to be dynamic.
    // In this case its used for the weather text description

    String message = model.getCurrentText() + " ";
    String messageArr[4];
    int variableRangeS = 0;
    int variableRangeE = 18;
    for (int i = 0; i < 4; i++) {
        while (message.substring(variableRangeE - 1, variableRangeE) != " ") {
            variableRangeE--;
        }
        messageArr[i] = message.substring(variableRangeS, variableRangeE);
        variableRangeS = variableRangeE;
        variableRangeE = variableRangeS + 18;
    }
    //=== OVERFLOW END ==============================

    m_manager.fillScreen(m_backgroundColor);
    String cityName = model.getCityName();
    cityName.remove(cityName.indexOf(",", 0));

    m_manager.setFontColor(m_foregroundColor);
    m_manager.drawFittedString(cityName, centre, 80, 210, 50, Align::MiddleCenter);

    auto y = 125;
    for (auto i = 0; i < 4; i++) {
        m_manager.drawCentreString(messageArr[i], centre, y, 15);
        y += 25;
    }
}

// Displays the next 3 days' weather forecast
void WeatherWidget::threeDayWeather(int displayIndex) {
    const int days = 3;
    const int columnSize = 75;
    const int highLowY = 210;

    m_manager.selectScreen(displayIndex);
    m_manager.fillScreen(m_backgroundColor);
    int fontSize = 22;

    // No glaring white chunks in Dark mode
    if (m_screenMode == Light) {
        m_manager.fillRect(0, 180, 240, 70, m_foregroundColor);
    }

    m_manager.drawString(m_mode == MODE_HIGHS ? "Highs" : "Lows", centre, highLowY, fontSize, Align::MiddleCenter, m_invertedForegroundColor, m_invertedBackgroundColor);
    // Reset colors
    m_manager.setFontColor(m_foregroundColor);
    m_manager.setBackgroundColor(m_backgroundColor);

    int temperatureFontSize = fontSize; // 0-9 only
    // Look up all the temperatures, and if any of them are more than 2 digits, we need
    // to scale down the font -- or it won't look right on the screen.
    String temps[days];
    for (auto i = 0; i < days; i++) {
        temps[i] = m_mode == MODE_HIGHS ? model.getDayHigh(i, 0) : model.getDayLow(i, 0);
        if (temps[i].length() > 4) {
            // We've got a nutty 3-digit temperature (plus degree sign), scale down
            temperatureFontSize = fontSize - 4; // smaller
        }
    }

    m_manager.setFontColor(m_foregroundColor);
    for (auto i = 0; i < days; i++) {
        // TODO: only works for 3 days
        const int x = (centre - columnSize) + i * columnSize;

        drawWeatherIcon(displayIndex, model.getDayIcon(i), x - 30, 40, 4);
        m_manager.drawCentreString(temps[i], x, 122, temperatureFontSize);

        String shortDayName = LOC_WEEKDAY[weekday(m_time->getUnixEpoch() + (86400 * (i + 1))) - 1];
        shortDayName.remove(3);
        m_manager.drawString(shortDayName, x, 154, fontSize, Align::MiddleCenter);
    }
}

int WeatherWidget::getClockStamp() {
    return m_time->getHour() * 60 + m_time->getMinute();
}

void WeatherWidget::configureColors() {
    m_foregroundColor = m_screenMode == Light ? TFT_BLACK : TFT_WHITE;
    m_backgroundColor = m_screenMode == Light ? TFT_WHITE : TFT_BLACK;

    // NOTE: In Light mode, we draw decorative black chunks and display the high and low on them in white.
    //       It does not make sense to have glaring white chunks in dark mode, so we don't draw them at all,
    //       and display the high and low in white too.
    m_invertedForegroundColor = m_screenMode == Light ? m_backgroundColor : m_foregroundColor;
    m_invertedBackgroundColor = m_screenMode == Light ? m_foregroundColor : m_backgroundColor;

    m_manager.setBackgroundColor(m_backgroundColor);
}

String WeatherWidget::getName() {
    return "Weather";
}
