#include <widget.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


class ClockWidget : public Widget {
public:
    ClockWidget(ScreenManager& manager);
    ~ClockWidget() override;
    void draw() override;
    void update() override;
private:

    void timeUpdate();
    void displayDidget(int displayIndex, String didget, int font, int fontSize, uint32_t color);

    time_t m_unixEpoch;
    int m_timeZoneOffset;

    WiFiUDP m_udp;
    NTPClient* m_timeClient{ nullptr };

    int m_minuteSingle;
    int m_hourSingle;

    int m_lastMinuteSingle;
    int m_lastHourSingle;

    // Didgets
    String m_display1Didget;
    String m_prevDisplay1Didget;    
    String m_display2Didget;
    String m_prevDisplay2Didget;
    //Display 3 is : 
    String m_display4Didget;
    String m_prevDisplay4Didget;
    String m_display5Didget;
    String m_prevDisplay5Didget;

    const int m_oneSecond{ 1000 };
    int m_updateTimer{ 0 };
};