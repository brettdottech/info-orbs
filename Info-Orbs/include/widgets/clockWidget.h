#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <widget.h>
#include <time.h>

class ClockWidget : public Widget {
public:
    ClockWidget(ScreenManager& manager);
    ~ClockWidget() override;
    void draw() override;
    void update() override;
private:

    void displayDidget(int displayIndex, String didget, int font, int fontSize, uint32_t color, bool shadowing);
    void displayDidget(int displayIndex, String didget, int font, int fontSize, uint32_t color);
    void displaySeconds(int displayIndex, int seconds, int color);

    time_t m_unixEpoch;
    int m_timeZoneOffset;

    WiFiUDP m_udp;
    NTPClient* m_timeClient{ nullptr };

    int m_minuteSingle;
    int m_hourSingle;
    int m_secondSingle;

    int m_lastMinuteSingle;
    int m_lastHourSingle;
    int m_lastSecondSingle;

    // Didgets
    String m_display1Didget;
    String m_lastDisplay1Didget;    
    String m_display2Didget;
    String m_lastDisplay2Didget;
    //Display 3 is : 
    String m_display4Didget;
    String m_lastDisplay4Didget;
    String m_display5Didget;
    String m_lastDisplay5Didget;
};
#endif // CLOCKWIDGET_H