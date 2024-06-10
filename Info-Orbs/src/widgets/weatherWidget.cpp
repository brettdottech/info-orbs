
#include "widgets/weatherWidget.h"

#include <config.h>

WeatherWidget::WeatherWidget(ScreenManager &manager) : Widget(manager) {
    for (int i = 0; i < 3; i++) {
        m_daysHigh[i] = 0.0;
        m_daysLow[i] = 0.0;
    }
    m_mode = MODE_HIGHS;
}

WeatherWidget::~WeatherWidget() {
}

void WeatherWidget::changeMode() {
    m_mode++;
    if (m_mode > MODE_LOWS) {
        m_mode = MODE_HIGHS;
    }
    draw(true);
}

void WeatherWidget::setup() {
    m_lastWeather = millis() - m_weatherDelay + 1000;
    m_weatherStamp = 0;
    m_time = GlobalTime::getInstance();
}

void WeatherWidget::draw(bool force) {
    m_time->updateTime();
    int clockStamp = getClockStamp();
    if (clockStamp != m_clockStamp || force) {
        displayClock(0, TFT_WHITE, TFT_BLACK);
        m_clockStamp = clockStamp;
    }

    // Weather, displays a clock, city & text weather discription, weather icon, temp, 3 day forecast
    int weatherStamp = getWeatherStamp();
    if ((weatherStamp != m_weatherStamp || force) && m_cityName != "") {
        weatherText(1, TFT_WHITE, TFT_BLACK);
        drawWeatherIcon(m_currentWeatherIcon, 2, 0, 0, 1);
        singleWeatherDeg(3, TFT_WHITE, TFT_BLACK);
        threeDayWeather(4);
        m_weatherStamp = weatherStamp;
    }
}

void WeatherWidget::update(bool force) {
    if (m_lastWeather == 0 || (millis() - m_lastWeather) >= m_weatherDelay) {
        setBusy(true);

        HTTPClient http;
        http.begin(httpRequestAddress);
        int httpCode = http.GET();

        if (httpCode > 0) {  // Check for the returning code
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, http.getString());
            if (!error) {
                m_cityName = doc["resolvedAddress"].as<String>();
                m_currentWeatherDeg = String((int) round(doc["currentConditions"]["temp"].as<float>()));
                m_currentWeatherText = doc["days"][0]["description"].as<String>();
                m_currentWeatherIcon = doc["currentConditions"]["icon"].as<String>();
                m_todayHigh = doc["days"][0]["tempmax"].as<float>();
                m_todayLow = doc["days"][0]["tempmin"].as<float>();
                for (int i = 0; i < 3; i++) {
                    m_daysIcons[i] = doc["days"][i + 1]["icon"].as<String>();
                    m_daysHigh[i] = doc["days"][i + 1]["tempmax"].as<float>();
                    m_daysLow[i] = doc["days"][i + 1]["tempmin"].as<float>();
                }
                m_lastWeather = millis();
            } else {
                // Handle JSON deserialization error
                Serial.println("deserializeJson() failed");
            }
        } else {
            // Handle HTTP request error
            Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
        setBusy(false);
    }
}

void WeatherWidget::displayClock(int displayIndex, uint32_t background, uint32_t color) {
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    int clky = 95;
    display.setTextColor(color);
    display.setTextSize(1);
    display.setTextDatum(MC_DATUM);

    display.fillScreen(background);
    display.setTextColor(color);
    display.setTextSize(2);
    display.setTextDatum(MC_DATUM);
#ifdef WEATHER_UNITS_METRIC
    display.drawString(String(m_time->getDay()) + " " + m_time->getMonthName(), centre, 151, 2);
#else
    display.drawString(m_time->getMonthName() + " " + String(m_time->getDay()), centre, 151, 2);
#endif
    display.setTextSize(3);
    display.drawString(m_time->getWeekday(), centre, 178, 2);
    display.setTextColor(color);
    display.setTextDatum(MR_DATUM);
    display.setTextSize(1);

    display.drawString(m_time->getHourPadded(), centre - 5, clky, 8);

    display.setTextColor(color);
    display.setTextDatum(ML_DATUM);
    display.setTextSize(1);
    display.drawString(m_time->getMinutePadded(), centre + 5, clky, 8);
    display.setTextDatum(MC_DATUM);
    display.setTextColor(color);
    display.drawString(":", centre, clky, 8);
}

// This will write an image to the screen when called from a hex array. Pass in:
// Screen #, X, Y coords, Bye Array To Pass, the sizeof that array, scale of the image(1= full size, then multiples of 2 to scale down)
// getting the byte array size is very annoying as its computed on compile so you cant do it dynamicly.
void WeatherWidget::showJPG(int displayIndex, int x, int y, const byte jpgData[], int jpgDataSize, int scale) {
    m_manager.selectScreen(displayIndex);

    TJpgDec.setJpgScale(scale);
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, jpgData, jpgDataSize);
    TJpgDec.drawJpg(x, y, jpgData, jpgDataSize);
}

// This takes the text output form the weatehr API and maps it to arespective icon/byte aarray, then displays it,
void WeatherWidget::drawWeatherIcon(String condition, int displayIndex, int x, int y, int scale) {
    const byte *icon = NULL;
    int size = 0;
    if (condition == "partly-cloudy-night") {
        icon = moonCloud;
        size = sizeof(moonCloud);
    } else if (condition == "partly-cloudy-day") {
        icon = sunClouds;
        size = sizeof(sunClouds);
    } else if (condition == "clear-day") {
        icon = sun;
        size = sizeof(sun);
    } else if (condition == "clear-night") {
        icon = moon;
        size = sizeof(moon);
    } else if (condition == "snow") {
        icon = snow;
        size = sizeof(snow);
    } else if (condition == "rain") {
        icon = rain;
        size = sizeof(rain);
    } else if (condition == "fog") {
        icon = clouds;
        size = sizeof(clouds);
    } else if (condition == "wind") {
        icon = clouds;
        size = sizeof(clouds);
    } else if (condition == "cloudy") {
        icon = clouds;
        size = sizeof(clouds);
    } else {
        Serial.println("unknown weather icon:" + condition);
    }
    if (icon != NULL && size > 0) {
        showJPG(displayIndex, x, y, icon, size, scale);
    }
}

// This displays the current weather temp on a single screen. Pass in display number, background color, text color
// dosent round deg, just removes all text after the decimil, should prob be fixed
void WeatherWidget::singleWeatherDeg(int displayIndex, uint32_t backgroundColor, uint32_t textColor) {
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();
    display.fillScreen(backgroundColor);

    drawDegrees(m_currentWeatherDeg, centre, 100, 8, 1, 15, 8, textColor, backgroundColor);


    display.fillRect(0, 170, 240, 70, TFT_BLACK);

    display.fillRect(centre-1, 170, 2, 240, TFT_WHITE);

    display.setTextDatum(MC_DATUM);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(2);
    display.drawString("High", 80, 190, 1);
    drawDegrees(String((int)round(m_todayHigh)), 80, 210, 1, 2, 4, 2, TFT_WHITE, TFT_BLACK);
    display.drawString("Low", 160, 190, 1);
    drawDegrees(String((int)round(m_todayLow)), 160, 210, 1, 2, 4, 2, TFT_WHITE, TFT_BLACK);
}

// This displays the users current city and the text desctiption of the weather. Pass in display number, background color, text color
void WeatherWidget::weatherText(int displayIndex, int16_t b, int16_t t) {
    m_manager.selectScreen(displayIndex);
    TFT_eSPI &display = m_manager.getDisplay();
    //=== TEXT OVERFLOW ============================
    // this takes a given string a and breaks it down in max x character long strings ensuring not to break it only at a space.
    // given the small width of the screens this will porbablly be needed to this project again so making sure to outline it
    // clearly as this should liekly eventually be turned into a fucntion. Before use the array size should be made to be dynamic.
    // In this case its used for the weather text description

    String message = m_currentWeatherText + " ";
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

    display.fillScreen(b);
    display.setTextColor(t);
    display.setTextSize(3);
    display.setTextDatum(MC_DATUM);
    m_cityName.remove(m_cityName.indexOf(",", 0));
    display.drawString(m_cityName, centre, 80, 2);
    display.setTextSize(2);
    display.setTextFont(1);
    display.drawString(messageArr[0], centre, 120);
    display.drawString(messageArr[1], centre, 140);
    display.drawString(messageArr[2], centre, 160);
    display.drawString(messageArr[3], centre, 180);
}

// This displays the next 3 days weather forecast
void WeatherWidget::threeDayWeather(int displayIndex) {
    m_manager.selectScreen(displayIndex);
    TFT_eSPI &display = m_manager.getDisplay();

    display.setTextDatum(MC_DATUM);
    display.fillScreen(TFT_WHITE);
    display.setTextSize(2);

    display.fillRect(78, 0, 3, 240, TFT_BLACK);
    display.fillRect(157, 0, 3, 240, TFT_BLACK);

    display.fillRect(0, 170, 240, 70, TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.drawString("Next 3 Days", centre, 191, 1);

    for (int i = 0; i < 3; i++) {
        int xOffset = (centre - 75) + i * 75;
        String temperature;
        display.setTextColor(TFT_WHITE);
        if (m_mode == MODE_HIGHS) {
            temperature = String((int) round(m_daysHigh[i]));
            display.drawString("Highs", centre, 215, 1);
        } else if (m_mode == MODE_LOWS) {
            temperature = String((int) round(m_daysLow[i]));
            display.drawString("Lows", centre, 215, 1);
        }
        drawWeatherIcon(m_daysIcons[i], displayIndex, xOffset - 30, 47, 4);
        display.setTextColor(TFT_BLACK);
        drawDegrees(temperature, xOffset, centre, 2, 2, 4, 2, TFT_BLACK, TFT_WHITE);

        String weekUpdate = dayStr(weekday(m_time->getUnixEpoch() + (86400 * (i + 1))));
        weekUpdate.remove(3);
        weekUpdate.toUpperCase();
        display.drawString(weekUpdate, xOffset, 150, 2);
    }
}

int WeatherWidget::drawDegrees(String number, int x, int y, uint8_t font, uint8_t size, uint8_t outerRadius, uint8_t innerRadius, int16_t textColor, int16_t backgroundColor) {
    TFT_eSPI &display = m_manager.getDisplay();

    display.setTextColor(textColor);
    display.setTextFont(font);
    display.setTextSize(size);
    display.setTextDatum(MC_DATUM);

    int16_t textWidth = display.textWidth(number);
    int16_t fontHeight = display.fontHeight(font);
    int offset = ceil(fontHeight * 0.15);
    int circleX = textWidth / 2 + x + offset;
    int circleY = y - fontHeight / 2 + floor(fontHeight/10);

    display.drawString(number, x, y, font);
    display.fillCircle(circleX, circleY, outerRadius, textColor);
    display.fillCircle(circleX, circleY, innerRadius, backgroundColor);

    return textWidth+offset;
}

int WeatherWidget::getClockStamp() {
    return m_time->getHour() * 60 + m_time->getMinute();
}

int WeatherWidget::getWeatherStamp() {
    return m_currentWeatherDeg.toInt() + m_daysHigh[0] * 100 + m_daysHigh[1] * 10000 + m_daysHigh[2] * 1000000;
}
