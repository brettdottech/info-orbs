#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include "GlobalTime.h"
#include "Widget.h"
#include "config_helper.h"
#include "nixie.h"

#define CLOCK_DEBUG

#ifndef CLOCK_NIXIE_COLOR
    #define CLOCK_NIXIE_COLOR 0
#endif

#ifndef USE_CLOCK_CUSTOM
    #define USE_CLOCK_CUSTOM 0 // no custom clocks by default
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
    NORMAL = 0,
    MORPH = 1,
    NIXIE = 2,
    CUSTOM0 = 3,
    CUSTOM1 = 4,
    CUSTOM2 = 5,
    CUSTOM3 = 6,
    CUSTOM4 = 7,
    CUSTOM5 = 8,
    CUSTOM6 = 9,
    CUSTOM7 = 10,
    CUSTOM8 = 11,
    CUSTOM9 = 12
};

#define CLOCK_TYPE_NUM 13

class ClockWidget : public Widget {
public:
    ClockWidget(ScreenManager &manager, ConfigManager &config);
    ~ClockWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    void addConfigToManager();
    void changeFormat();
    void changeZone(int dir);
    void displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color, bool shadowing);
    void displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color);
    void displaySeconds(int displayIndex, int seconds, int color);
    void displayAmPm(String &amPm, uint32_t color);
    DigitOffset getOffsetForDigit(const String &digit);
    void displayDigitImage(int displayIndex, const String &digit);
    void displayNixie(int displayIndex, uint8_t index);
    void displayCustom(int displayIndex, uint8_t clockNumber, uint8_t index);
    void displayClockGraphics(int displayIndex, const byte *clockArray[12][2], uint8_t index, int colorOverride);
    void changeClockType();
    bool isValidClockType(int clockType);
    bool isCustomClock(int clockType);
    void displayTZ(uint32_t color);
	void displayMorphDigit(int orb, const String m_digitlast, const String m_digit, uint32_t color);
	void drawDisappearRL(int seg, int seq);
	void drawDisappearLR(int seg, int seq);
	void drawAppearLR(int seg, int seq);
	void drawAppearRL(int seg, int seq);
	void drawAppearBT(int seg, int seq);
	void drawAppearTB(int seg, int seq);
	void drawDisappearTB(int seg, int seq);
	void drawDisappearBT(int seg, int seq);
	void drawSegment(int seg, uint32_t color);

    int m_type = (int) DEFAULT_CLOCK;

    int m_format = CLOCK_FORMAT;
    bool m_showSecondTicks = SHOW_SECOND_TICKS;
    int m_fgColor = CLOCK_COLOR;
    int m_shadowColor = CLOCK_SHADOW_COLOR;
    bool m_shadowing = CLOCK_SHADOWING;
    int m_overrideNixieColor = CLOCK_NIXIE_COLOR;
    bool m_enableNixie = USE_CLOCK_NIXIE > 0;

    // Colors for CustomClocks
    int m_customTickColor[10]{TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE, TFT_WHITE};
    int m_customOverrideColor[10]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool m_customEnabled[10]{false, false, false, false, false, false, false, false, false, false};

    time_t m_unixEpoch;
    int m_timeZoneOffset;
    int m_timeZone = 0;
    int m_timeZonePrev = 4;

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
    // Display 3 is ':'
    String m_display4Digit;
    String m_lastDisplay4Digit{""};
    String m_display5Digit;
    String m_lastDisplay5Digit{""};

    String m_amPm;
    String m_lastAmPm{""};

    DigitOffset m_digitOffsets[10] = CLOCK_DIGITS_OFFSET;

	// Stuff for Morph Clock
    int animDelay = 35; // Set the animation speed in ms per step and 5 steps per segment
    bool m_enableMorph = USE_CLOCK_MORPH;
//    #define animDelay 35 // Set the animation speed in ms per step and 5 steps per segment
	// Segment location in format : X , Y, L, W
	const int segLoc[7][4] = { 
		{18,0,80,16},      //A
		{100,10,16,80},    //B
		{100,96,16,80},    //C
		{18,170,80,16},    //D
		{00,96,16,80},     //E
		{00,10,16,80},     //F
		{18,84,80,16}};    //G

	// Which segments for each digit
	const int digits[10][7] = {
		{1,1,1,1,1,1,0},    //0
		{0,1,1,0,0,0,0},    //1
		{1,1,0,1,1,0,1},    //2
		{1,1,1,1,0,0,1},    //3
		{0,1,1,0,0,1,1},    //4
		{1,0,1,1,0,1,1},    //5
		{1,0,1,1,1,1,1},    //6
		{1,1,1,0,0,0,0},    //7
		{1,1,1,1,1,1,1},    //8
		{1,1,1,1,0,1,1}};   //9

	const int trans_0_2[2][7] = {
        {0,0,2,0,0,2,3},    //2-0 
		{4,0,2,4,1,0,4}};   //2-1
	const int trans_0_3[1][7] = {{0,0,0,0,2,2,3}};   //3-0
	const int trans_0_5[1][7] = {{0,2,0,0,2,0,3}};   //5-0
	const int trans_0_9[10][7] = {     //Key is current digit
		{0,0,0,0,2,0,3},    //9-0
		{4,0,0,4,4,4,0},    //0-1
		{5,0,0,5,5,0,5},    //1-2
		{0,0,6,0,0,0,0},    //2-3
		{3,0,0,4,0,2,0},    //3-4
		{5,1,0,5,0,0,0},    //4-5
		{0,0,0,0,5,0,0},    //5-6
		{0,6,0,4,4,4,4},    //6-7
		{0,0,0,5,5,5,5},    //7-8
		{0,0,0,0,4,0,0}};   //8-9
	const int trans_0_B[2][7] = {     //Key is current digit
		{9,7,1,9,9,9,9},    //1-  
		{9,8,2,9,9,9,9}};   // -1
};
#endif // CLOCKWIDGET_H
