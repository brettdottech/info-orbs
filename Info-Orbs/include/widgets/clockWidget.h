#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <globalTime.h>
#include <widget.h>

class ClockWidget : public Widget {
   public:
    ClockWidget(ScreenManager& manager);
    ~ClockWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void changeMode() override;

   private:
    void displayDidget(int displayIndex, const String& didget, int font, int fontSize, uint32_t color, bool shadowing);
    void displayDidget(int displayIndex, const String& didget, int font, int fontSize, uint32_t color);
    void displaySeconds(int displayIndex, int seconds, int color);
    void displayAmPm(uint32_t color);

    time_t m_unixEpoch;
    int m_timeZoneOffset;

    // Delays for setting how often certain screens/functions are refreshed/checked. These include both the frequency which they need to be checked and a varibale to store the last checked value.
    long m_secondTimer = 2000;  // this time is used to refressh/check the clock every second.
    long m_secondTimerPrev = 0;

    // WiFiUDP m_udp;
    // NTPClient* m_timeClient{ nullptr };

    int m_minuteSingle;
    int m_hourSingle;
    int m_secondSingle;

    int m_lastMinuteSingle{-1};
    int m_lastHourSingle{-1};
    int m_lastSecondSingle{-1};

    // Didgets
    String m_display1Didget;
    String m_lastDisplay1Didget{"-1"};
    String m_display2Didget;
    String m_lastDisplay2Didget{"-1"};
    // Display 3 is :
    String m_display4Didget;
    String m_lastDisplay4Didget{"-1"};
    String m_display5Didget;
    String m_lastDisplay5Didget{"-1"};
};
#endif  // CLOCKWIDGET_H
