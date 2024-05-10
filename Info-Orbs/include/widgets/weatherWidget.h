#ifndef WEAHTERWIDGET_H
#define WEAHTERWIDGET_H

#include <widget.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class WeatherWidget : public Widget {
public:
    WeatherWidget(ScreenManager& manager);
    ~WeatherWidget() override;
    void setup() override;
    void update() override;
    void draw() override;
private:

    void displayClock(int displayIndex, String time, String monthName, int day, String weekday, int color);

    String m_time;
    String m_lastTime;
    String m_monthName;
    String m_weekday;
    int m_day;


};


#endif // WEAHTERWIDGET_H