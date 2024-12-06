#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#define MAX_WRAPPED_LINES 10

enum ScreenMode {
    Light = 1,
    Dark = 2
};

#define ScreenWidth 240
#define ScreenHeight 240
#define ScreenCenterX (ScreenWidth / 2)
#define ScreenCenterY (ScreenHeight / 2)

class Utils {
public:
    static int getWrappedLines(String (&lines)[MAX_WRAPPED_LINES], String str, int limit);
    static String getWrappedLine(String str, int limit, int lineNum, int maxLines);
    static int32_t stringToColor(String color);
    static String formatFloat(float value, int8_t digits);
    static int32_t stringToAlignment(String alignment);
    static uint16_t rgb565dim(uint16_t rgb565, uint8_t brightness, bool swapBytes = false);
    static uint32_t rgb565ToRgb888(uint16_t rgb565, bool swapBytes);
    static uint16_t rgb888ToRgb565(uint32_t rgb888, bool swapBytes);
    static uint16_t grayscaleToTargetColor(uint8_t grayscale, uint32_t targetColor888, float brightness, bool swapBytes);
    static void colorizeImageData(uint16_t *pixels565, size_t length, uint32_t targetColor888, float brightness, bool swapBytes);
};

#endif
