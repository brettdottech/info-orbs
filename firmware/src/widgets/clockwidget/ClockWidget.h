#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include "GlobalTime.h"
#include "Widget.h"
#include <TJpg_Decoder.h>

#ifndef USE_CLOCK_NIXIE
    #define USE_CLOCK_NIXIE true
#endif

#ifndef USE_CLOCK_CUSTOM
    #define USE_CLOCK_CUSTOM false
#endif

#if USE_CLOCK_NIXIE
    #include "nixie.h"
#endif

#if USE_CLOCK_CUSTOM
    #include "clock-custom.h"
#endif

#ifndef CLOCK_FONT
    #define CLOCK_FONT DSEG7
#endif

#ifndef CLOCK_FONT_SIZE
    #define CLOCK_FONT_SIZE 200
#endif

// Some fonts are not really centered, use this to compensate
#ifndef CLOCK_OFFSET_X_DIGITS
    #define CLOCK_OFFSET_X_DIGITS -10
#endif

#ifndef CLOCK_OFFSET_X_COLON
    #define CLOCK_OFFSET_X_COLON 0
#endif

// Not all digits in DSEG7/14 are aligned identically, therefore we need to offset them
#ifndef CLOCK_DIGITS_OFFSET
    #define CLOCK_DIGITS_OFFSET \
        {                       \
            {0, 0}, {1, -5}, {0, 0}, {1, 0}, {0, -5}, {0, 0}, {0, 0}, {0, -5}, {0, 0}, {0, 0}}
// Use zero offset for other fonts (or adjust)
// #define CLOCK_DIGITS_OFFSET { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
#endif

#ifndef CLOCK_COLOR
    #define CLOCK_COLOR FOREGROUND_COLOR
#endif

#ifndef CLOCK_SHADOW_COLOR
    #define CLOCK_SHADOW_COLOR BG_COLOR
#endif

#ifndef CLOCK_SHADOWING
    #define CLOCK_SHADOWING SHADOWING
#endif

#ifndef DEFAULT_CLOCK
    #define DEFAULT_CLOCK ClockType::NORMAL
#endif

struct DigitOffset {
    int x;
    int y;
};

enum class ClockType {
    NORMAL,
    NIXIE,
    CUSTOM
};

class ClockWidget : public Widget {
public:
    ClockWidget(ScreenManager &manager);
    ~ClockWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    void change24hMode();
    void displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color, bool shadowing);
    void displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color);
    void displaySeconds(int displayIndex, int seconds, int color);
    void displayAmPm(String &amPm, uint32_t color);
    DigitOffset getOffsetForDigit(const String &digit);
    void displayImage(int displayIndex, const String &digit);
    void displayNixie(int displayIndex, const String &digit);
    void displayCustom(int displayIndex, const String &digit);
    void changeClockType();

    ClockType m_type = DEFAULT_CLOCK;

    time_t m_unixEpoch;
    int m_timeZoneOffset;

    // Delays for setting how often certain screens/functions are refreshed/checked. These include both the frequency which they need to be checked and a varibale to store the last checked value.
    unsigned long m_secondTimer = 2000; // This time is used to refressh/check the clock every second.
    unsigned long m_secondTimerPrev = 0;

    int m_minuteSingle;
    int m_hourSingle;
    int m_secondSingle;

    int m_lastMinuteSingle{-1};
    int m_lastHourSingle{-1};
    int m_lastSecondSingle{-1};

    // Digits
    String m_display1Digit;
    String m_lastDisplay1Digit{""};
    String m_display2Digit;
    String m_lastDisplay2Digit{""};
    // Display 3 is :
    String m_display4Digit;
    String m_lastDisplay4Digit{""};
    String m_display5Digit;
    String m_lastDisplay5Digit{""};

    String m_amPm;
    String m_lastAmPm{""};

    DigitOffset m_digitOffsets[10] = CLOCK_DIGITS_OFFSET;
};
#endif // CLOCKWIDGET_H
