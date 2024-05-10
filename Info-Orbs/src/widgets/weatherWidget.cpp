
#include "widgets/weatherWidget.h"
#include <user.h>
#include <globalTime.h>

WeatherWidget::WeatherWidget(ScreenManager &manager): Widget(manager) {
}

WeatherWidget::~WeatherWidget() {

}

void WeatherWidget::setup() {
    m_lastTime = "-1";
}


void WeatherWidget::draw() {
    if(m_time != m_lastTime) {
        displayClock(0, m_time, m_monthName, m_day, m_weekday, TFT_WHITE);
        m_lastTime = m_time;
    }
}

void WeatherWidget::update() {
    GlobalTime* time = GlobalTime::getInstance();
    m_time = time->getTime();
    m_monthName = time->getMonthName();
    m_day = time->getDay();
    m_weekday = time->getWeekday();
}


void WeatherWidget::displayClock(int displayIndex, String time, String monthName, int day, String weekday, int color){
    m_manager.selectScreen(displayIndex);

    TFT_eSPI& display = m_manager.getDisplay();
    display.fillScreen(TFT_BLACK);
    display.setTextColor(color);
    display.setTextDatum(MC_DATUM);

    display.setTextSize(1);
    display.drawString(time, 120, 80, 6);
    display.drawString(monthName, 160, 120, 4);
    display.drawString(String(day), 80, 120, 4);
    display.drawString(weekday, 120, 180, 4);
}


