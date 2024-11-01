#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#define MAX_WRAPPED_LINES 10

enum ScreenMode { 
    Light = 1,
    Dark = 2
};

class Utils {
public:
    static int getWrappedLines(String (&lines)[MAX_WRAPPED_LINES], String str, int limit);
    static String getWrappedLine(String str, int limit, int lineNum, int maxLines);
    static int32_t stringToColor(String color);
    static String formatFloat(float value, int8_t digits);
    static int32_t stringToAlignment(String alignment);
};

#endif
