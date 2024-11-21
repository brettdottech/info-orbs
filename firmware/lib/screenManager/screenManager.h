#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// Include any necessary libraries here
#include <config_helper.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <OpenFontRender.h>
#include "../../include/ttffonts.h"

#define NUM_SCREENS 5

#ifndef DEFAULT_FONT
#define DEFAULT_FONT ROBOTO_REGULAR
#endif

enum TTF_Font {
    NONE,
    ROBOTO_REGULAR,
    FINAL_FRONTIER,
    DSEG7,
    DSEG14
};

struct TTF_FontScale {
    TTF_Font font;
    float scale;
};

const TTF_FontScale fontScaleFactors[] = { { ROBOTO_REGULAR, 1.37 }, { DSEG14, 1}, { DSEG14, 1}, { FINAL_FRONTIER, 1.5 } };

class ScreenManager {
public:
    ScreenManager(TFT_eSPI& tft);

    void setFont(TTF_Font font);
    TFT_eSPI& getDisplay();
    OpenFontRender &getRender();

    void selectScreen(int screen);
    void selectAllScreens();
    void reset();

    void fillAllScreens(uint32_t color);
    void clearAllScreens();
    void setFontColor(uint32_t color);
    void setFontColor(uint32_t color, uint32_t background);
    void setBackgroundColor(uint32_t color);
    void setFontSize(uint32_t size);
    void setAlignment(Align align);

    void fillScreen(uint32_t color);
    void clearScreen(int screen=-1);

    unsigned int calculateFitFontSize(uint32_t limit_width, uint32_t limit_height, Layout layout, const char *str);

    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align, u_int16_t fgColor, uint16_t bgColor, bool applyScale=true);
    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align, u_int16_t fgColor);
    void drawString(const char *text, int x, int y, unsigned int fontSize, Align align);
    void drawString(const char *text, int x, int y);

    void drawCentreString(const char *text, int x, int y, unsigned int fontSize=0);

    void drawFittedString(const char *text, int x, int y, int limit_w, int limit_h, Align align);
    void drawFittedString(const char *text, int x, int y, int limit_w, int limit_h);

    // Drawing stuff
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color);
    void drawArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool smoothArc = true);
    void drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool roundEnds = false);
    void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);

private:
    unsigned int getScaledFontSize(unsigned int fontSize);
    uint8_t m_screen_cs[5] = {SCREEN_1_CS, SCREEN_2_CS, SCREEN_3_CS, SCREEN_4_CS, SCREEN_5_CS};
    TFT_eSPI& m_tft;
    OpenFontRender m_render;
    TTF_Font m_curFont = TTF_Font::NONE;
};

#endif // SCREENMANAGER_H