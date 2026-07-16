#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// Include any necessary libraries here
#include "config_helper.h"
#include "ttf-fonts.h"
#include <OpenFontRender.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

#define NUM_SCREENS 5

#ifndef DEFAULT_FONT
    #define DEFAULT_FONT ROBOTO_REGULAR
#endif

#ifndef TFT_BRIGHTNESS
    #define TFT_BRIGHTNESS 255
#endif

class ScreenManager {
public:
    ScreenManager(TFT_eSPI &tft);

    void selectScreen(int screen);
    void selectAllScreens();
    void reset();

    void fillAllScreens(uint32_t color);
    void clearAllScreens();
    void fillScreen(uint32_t color);
    void clearScreen(int screen = -1);

    bool setBrightness(uint8_t brightness);
    uint8_t getBrightness();

    // Set TTF parameters for next drawString()
    void setFont(TTF_Font font);
    void setFontColor(uint32_t color);
    void setFontColor(uint32_t color, uint32_t background);
    void setBackgroundColor(uint32_t color);
    void setFontSize(uint32_t size);
    void setAlignment(Align align);

    // Helper functions
    unsigned int calculateFitFontSize(uint32_t limit_width, uint32_t limit_height, Layout layout, const String &text);

    // Draw string functions
    void drawString(const String &text, int x, int y, unsigned int fontSize, Align align, int32_t fgColor = -1, int32_t bgColor = -1, bool applyScale = true);
    void drawString(const String &text, int x, int y);

    // Draw centered string
    void drawCentreString(const String &text, int x, int y, unsigned int fontSize = 0);

    // Draw string with a max width/height (auto-sizing)
    void drawFittedString(const String &text, int x, int y, int limit_w, int limit_h, Align align);
    void drawFittedString(const String &text, int x, int y, int limit_w, int limit_h);

    // Drawing stuff
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color);
    void drawArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool smoothArc = true);
    void drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool roundEnds = false);
    void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
    void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);

    // Legacy text function (not using TTF)
    int16_t getLegacyFontHeight();
    void setLegacyTextColor(uint16_t color);
    void setLegacyTextColor(uint16_t fgcolor, uint16_t bgcolor, bool bgfill = false);
    void setLegacyTextDatum(uint8_t datum);
    void setLegacyTextSize(uint8_t size);
    void setLegacyTextFont(uint8_t font);
    void drawLegacyString(const String &string, int32_t x, int32_t y);
    void drawLegacyString(const String &string, int32_t x, int32_t y, uint8_t font);
    int16_t drawLegacyChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font);

    // Image functions
    static bool tftOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
    JRESULT drawJpg(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t data_size, uint8_t scale = 1, uint32_t imageColor = 0);
    JRESULT drawFsJpg(int32_t x, int32_t y, const char *filename, uint8_t scale = 1, uint32_t imageColor = 0);

    // Additional functions used by MatrixWidget
    int16_t width();
    int16_t height();
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t b);
    void setTextColor(uint16_t c, uint16_t b, bool bgfill);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t s);
    void print(String s);
    void print(char c);
    // Color conversion
    uint16_t color565FromHex(const String &hex);

    // Title bar drawing
    void drawTitleBars(uint16_t topColor, uint16_t bottomColor,
                       const String &topText = "", const String &bottomText = "",
                       uint16_t topTextColor = TFT_WHITE, uint16_t bottomTextColor = TFT_WHITE,
                       uint8_t topHeight = 30, uint8_t bottomHeight = 30,
                       uint8_t topTextSize = 16, uint8_t bottomTextSize = 16);

private:
    static ScreenManager *instance;

    uint8_t m_screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI &m_tft;
    OpenFontRender m_render;
    TTF_Font m_curFont = TTF_Font::NONE;
    uint8_t m_brightness = TFT_BRIGHTNESS;
    uint32_t m_imageColor = 0;

    TFT_eSPI &getDisplay();
    OpenFontRender &getRender();
    unsigned int getScaledFontSize(unsigned int fontSize);
    uint16_t dim(uint16_t color);
};

#endif // SCREENMANAGER_H
