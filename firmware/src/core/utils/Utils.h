#ifndef UTILS_H
#define UTILS_H

#include "Button.h"
#include <Arduino.h>
#include <string>

#define MAX_WRAPPED_LINES 10

enum ScreenMode {
    Light = 0,
    Dark = 1
};

// Widget states
#define WIDGET_DISABLED 0
#define WIDGET_OFF 1
#define WIDGET_ON 2

enum class TimeFrequency : unsigned long {
    Immediate = 0,
    OneHundredMilliseconds = 100,
    FiveHundredMilliseconds = 500,
    OneSecond = 1000,
    TwoSeconds = 2000,
    ThreeSeconds = 3000,
    FourSeconds = 4000,
    FiveSeconds = 5000,
    TenSeconds = 10000,
    FifteenSeconds = 15000,
    TwentySeconds = 20000,
    TwentyFiveSeconds = 25000,
    ThirtySeconds = 30000,
    OneMinute = 60000,
    TwoMinutes = 120000,
    ThreeMinutes = 180000,
    FourMinutes = 240000,
    FiveMinutes = 300000,
    TenMinutes = 600000,
    FifteenMinutes = 900000,
    ThirtyMinutes = 1800000,
    OneHour = 3600000,
    TwoHours = 7200000,
    FourHours = 14400000,
    SixHours = 21600000,
    TwelveHours = 43200000,
    OneDay = 86400000,
    OneWeek = 604800000
};

#define ScreenWidth 240
#define ScreenHeight 240
#define ScreenCenterX (ScreenWidth / 2)
#define ScreenCenterY (ScreenHeight / 2)

class Utils {
public:
    static void setBusy(bool busy);
    static int getWrappedLines(String (&lines)[MAX_WRAPPED_LINES], String str, int limit);
    static String getWrappedLine(String str, int limit, int lineNum, int maxLines);
    static int32_t stringToColor(String color);
    static String formatFloat(float value, int8_t digits);
    static int32_t stringToAlignment(String alignment);

    static uint16_t rgb565dim(uint16_t rgb565, uint8_t brightness, bool swapBytes = false);
    static void rgb565dimBitmap(uint16_t *pixel565, size_t length, uint8_t brightness, bool swapBytes = true);
    static uint32_t rgb565ToRgb888(uint16_t rgb565, bool swapBytes = false);
    static uint16_t rgb888ToRgb565(uint32_t rgb888, bool swapBytes = false);
    static String rgb565ToRgb888html(int color565);
    static int rgb888htmlToRgb565(String hexColor);
    static uint16_t grayscaleToTargetColor(uint8_t grayscale, uint8_t targetR8, uint8_t targetG8, uint8_t targetB8, float brightness, bool swapBytes = false);
    static void colorizeImageData(uint16_t *pixels565, size_t length, uint32_t targetColor888, float brightness, bool swapBytes = true);

    static Buttons stringToButtonId(const String &buttonName);
    static ButtonState stringToButtonState(const String &buttonState);

    /**
     * Create a new char buffer and copy the contents of the string into it.
     * Warning: This will allocate memory! Remeber to delete[] it after use.
     */
    static const char *createConstCharBuffer(const std::string &originalString);
    /**
     * Create a new char buffer and copy the contents of the strings into it.
     * Warning: This will allocate memory! Remeber to delete[] it after use.
     */
    static const char *createConstCharBufferAndConcat(const char *prefix, const char *original, const char *postfix);
};

#endif
