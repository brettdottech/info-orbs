#include <widget.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


class ClockWidget : public Widget {
public:
    ClockWidget(const ScreenManager& manager);
    ~ClockWidget();
    void draw();
    void update();
private:

    void timeUpdate();

    unsigned long m_unixEpoch;
    int m_timeZoneOffset;

    WiFiUDP m_udp;
    NTPClient* m_timeClient{ nullptr };

    String m_minuteSingle;
    String m_hourSingle;
    String m_daySingle;

    String m_weekSingle;
    String m_monthSingle;

    const int m_oneSecond{ 1000 };
    int m_updateTimer{ 0 };
};