
#include "widgets/weatherWidget.h"
#include <config.h>
#include <globalTime.h>

WeatherWidget::WeatherWidget(ScreenManager &manager) : Widget(manager) {
    daysDegs[0] = "0";
    daysDegs[1] = "0";
    daysDegs[2] = "0";
    m_hour = "0";
    m_minute = "0";
}

WeatherWidget::~WeatherWidget() {
}

void WeatherWidget::setup() {
    m_lastTime = 0;
    m_lastWeather = 0;
    m_weatherStamp = 0;
    m_clockStamp = 0;
}

void WeatherWidget::draw() {
    draw(false);
}

void WeatherWidget::draw(bool force) {
    int clockStamp = getClockStamp();
    if ((clockStamp != m_clockStamp || force) && m_day.toInt() != 0) {
        displayClock(0, TFT_WHITE);
        m_clockStamp = clockStamp;
    }

    // Weather, displays a clock, city & text weather discription, weather icon, temp, 3 day forecast
    int weatherStamp = getWeatherStamp();
    if ((weatherStamp != m_weatherStamp || force) && cityName != "") {
        weatherText(1, TFT_WHITE, TFT_BLACK);
        drawWeatherIcon(m_currentWeatherIcon, 2, 0, 0, 1);
        singleWeatherDeg(3, TFT_WHITE, TFT_BLACK);
        threeDayWeather(4);
        m_weatherStamp = weatherStamp;
    }
}

void WeatherWidget::update() {
    update(false);
}

void WeatherWidget::update(bool force) {
    GlobalTime* time = GlobalTime::getInstance();
    time->updateTime();
    m_hour = time->getHour();
    m_minute = time->getMinute();
    m_monthName = time->getMonthName();
    m_day = time->getDay();
    m_weekday = time->getWeekday();

    if (m_lastWeather == 0 || (millis() - m_lastWeather) >= m_weatherDelay) {
        HTTPClient http;
        int httpCode;
        http.begin(httpRequestAddress);
        httpCode = http.GET();
        JsonDocument doc;
        deserializeJson(doc, http.getString());
        if (doc["resolvedAddress"].as<String>() != "null") {
            timeZoneOffSet = doc["tzoffset"].as<int>();
            time->setTimeZoneOffset(timeZoneOffSet);
            cityName = doc["resolvedAddress"].as<String>();
            currentWeatherDeg = doc["currentConditions"]["temp"].as<String>();
            currentWeatherText = doc["days"][0]["description"].as<String>();
            m_currentWeatherIcon = doc["currentConditions"]["icon"].as<String>();
            daysIcons[0] = doc["days"][1]["icon"].as<String>();
            daysDegs[0] = doc["days"][1]["temp"].as<String>();
            daysIcons[1] = doc["days"][2]["icon"].as<String>(); // I hade these set up on a loop but I ran out of DRAM and had some varibles returning Nulls removing the loop fixed it however there is allot to optomize here.
            daysDegs[1] = doc["days"][2]["temp"].as<String>();  // I think the API call is too heavy, may need to be filtered before calling using the arduinoJson library, which is built in.
            daysIcons[2] = doc["days"][3]["icon"].as<String>();
            daysDegs[2] = doc["days"][3]["temp"].as<String>();
            m_lastWeather = millis();
        }
        http.end();
    }
}

void WeatherWidget::displayClock(int displayIndex, int color) {
    m_manager.selectScreen(displayIndex);
    uint32_t background = TFT_BLACK;

    TFT_eSPI &display = m_manager.getDisplay();

    int clky = 95;
    display.setTextColor(color);
    display.setTextSize(1);
    display.setTextDatum(MC_DATUM);

    display.fillScreen(background);
    display.setTextColor(color);
    display.setTextSize(2);
    display.setTextDatum(MC_DATUM);
    display.drawString(m_monthName + " " + m_day, centre, 151, 2);
    display.setTextSize(3);
    display.drawString(m_weekday, centre, 178, 2);
    display.setTextColor(color);
    display.setTextDatum(MR_DATUM);
    display.setTextSize(1);

    if (m_hour.length() == 2) {
        display.drawString(m_hour, centre - 5, clky, 8);
    } else {
        display.drawString("0" + m_hour, centre - 5, clky, 8);
    }

    display.setTextColor(color);
    display.setTextDatum(ML_DATUM);
    display.setTextSize(1);
    if (m_minute.length() == 2) {
        display.drawString(m_minute, centre + 5, clky, 8);
    } else {
        display.drawString("0" + m_minute, centre + 5, clky, 8);
    }
    display.setTextDatum(MC_DATUM);
    display.setTextColor(color);
    display.drawString(":", centre, clky, 8);
}

// This will wrie an image to the screen when called from a hex array. Pass in:
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
    Serial.print("drawWeatherIcon: ");
    Serial.println(condition);
    if (condition == "partly-cloudy-night") {
        showJPG(displayIndex, x, y, moonCloud, sizeof(moonCloud), scale);
    } else if (condition == "partly-cloudy-day") {
        showJPG(displayIndex, x, y, sunClouds, sizeof(sunClouds), scale);
    } else if (condition == "clear-day") {
        showJPG(displayIndex, x, y, sun, sizeof(sun), scale);
    } else if (condition == "clear-night") {
        showJPG(displayIndex, x, y, moon, sizeof(moon), scale);
    } else if (condition == "snow") {
        showJPG(displayIndex, x, y, snow, sizeof(snow), scale);
    } else if (condition == "rain") {
        showJPG(displayIndex, x, y, rain, sizeof(rain), scale);
    } else if (condition == "fog") {
        showJPG(displayIndex, x, y, clouds, sizeof(clouds), scale);
    } else if (condition == "wind") {
        showJPG(displayIndex, x, y, clouds, sizeof(clouds), scale);
    } else if (condition == "cloudy") {
        showJPG(displayIndex, x, y, clouds, sizeof(clouds), scale);
    }
}

// This displays the current weather temp on a single screen. Pass in display number, background color, text color
// dosent round deg, just removes all text after the decimil, should prob be fixed
void WeatherWidget::singleWeatherDeg(int displayIndex, uint32_t backgroundColor, uint32_t textColor) {
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    display.fillScreen(backgroundColor);
    display.setTextColor(textColor);
    display.setTextSize(1);
    display.setTextDatum(MC_DATUM);
    currentWeatherDeg.remove(currentWeatherDeg.indexOf(".", 0));
    display.drawString(currentWeatherDeg, centre, centre, 8);
    display.setTextFont(8);
    display.fillCircle(display.textWidth(currentWeatherDeg) / 2 + centre + 10, centre - display.fontHeight(8) / 2, 15, textColor);
    display.fillCircle(display.textWidth(currentWeatherDeg) / 2 + centre + 10, centre - display.fontHeight(8) / 2, 7, backgroundColor);
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

    String message = currentWeatherText + " ";
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
    cityName.remove(cityName.indexOf(",", 0));
    display.drawString(cityName, centre, 80, 2);
    Serial.println(cityName);
    Serial.println(currentWeatherText);
    display.setTextSize(2);
    display.drawString(messageArr[0], centre, 120, displayIndex); // BUG? displayIndex is likely misused
    display.drawString(messageArr[1], centre, 140, displayIndex); // BUG? displayIndex is likely misused
    display.drawString(messageArr[2], centre, 160, displayIndex); // BUG? displayIndex is likely misused
    display.drawString(messageArr[3], centre, 180, displayIndex); // BUG? displayIndex is likely misused
}

// This displays the next 3 days weather forecast
void WeatherWidget::threeDayWeather(int displayIndex) {
    m_manager.selectScreen(displayIndex);
    TFT_eSPI &display = m_manager.getDisplay();

    GlobalTime *time = GlobalTime::getInstance();

    display.fillScreen(TFT_WHITE);
    display.setTextSize(2);
    drawWeatherIcon(daysIcons[0], displayIndex, 90 - 75, 47, 4);
    drawWeatherIcon(daysIcons[1], displayIndex, 90, 47, 4);
    drawWeatherIcon(daysIcons[2], displayIndex, 90 + 75, 47, 4);

    display.setTextColor(TFT_BLACK);
    display.drawLine(78, 0, 78, 240, TFT_BLACK);
    display.drawLine(79, 0, 79, 240, TFT_BLACK);
    display.drawLine(80, 0, 80, 240, TFT_BLACK);
    display.drawLine(157, 0, 157, 240, TFT_BLACK);
    display.drawLine(158, 0, 158, 240, TFT_BLACK);
    display.drawLine(159, 0, 159, 240, TFT_BLACK);
    daysDegs[0].remove(daysDegs[0].indexOf(".", 0)); // remove decimal from deg, this dosent round just removes it, should probally fix this and also store as a different var type
    display.drawString(daysDegs[0], centre - 75, 120, 2);
    display.drawCircle(60, centre - display.fontHeight(2) / 2, 4, TFT_BLACK);

    String weekUpdate = dayStr(weekday(time->getUnixEpoch() + 86400)); // Adds 1 day to time
    weekUpdate.remove(3);
    weekUpdate.toUpperCase();
    display.drawString(weekUpdate, centre - 75, 150, 2);

    daysDegs[1].remove(daysDegs[1].indexOf(".", 0));
    display.drawString(daysDegs[1], centre, 120, 2);
    display.drawCircle(132, centre - display.fontHeight(2) / 2, 4, TFT_BLACK);
    weekUpdate = dayStr(weekday(time->getUnixEpoch() + 172800));
    weekUpdate.remove(3);
    weekUpdate.toUpperCase();
    display.drawString(weekUpdate, centre, 150, 2);

    daysDegs[2].remove(daysDegs[2].indexOf(".", 0));
    display.drawString(daysDegs[2], centre + 75, 120, 2);
    weekUpdate = dayStr(weekday(time->getUnixEpoch() + 259200));
    weekUpdate.remove(3);
    weekUpdate.toUpperCase();
    display.drawCircle(207, centre - display.fontHeight(2) / 2, 4, TFT_BLACK); // Had a variable offset on these but broke so I removed it
    display.drawString(weekUpdate, centre + 75, 150, 2);
    display.fillRect(0, 180, 240, 70, TFT_RED);
    display.setTextColor(TFT_WHITE);
    display.drawString("Next 3 Days..", centre, 201, 1);
}

int WeatherWidget::getClockStamp() {
    return m_hour.toInt() * 60 + m_minute.toInt();
}

int WeatherWidget::getWeatherStamp() {
    return currentWeatherDeg.toInt() + daysDegs[0].toInt() * 100 + daysDegs[1].toInt() * 10000 + daysDegs[2].toInt() * 1000000;
}
