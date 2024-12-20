#include "ScreenManager.h"
#include "ConfigManager.h"
#include "Utils.h"
#include <Arduino.h>

ScreenManager *ScreenManager::instance = nullptr;

ScreenManager::ScreenManager(TFT_eSPI &tft) : m_tft(tft) {

    for (int i = 0; i < NUM_SCREENS; i++) {
        pinMode(m_screen_cs[i], OUTPUT);
        digitalWrite(m_screen_cs[i], LOW);
    }

    m_tft.init();
    m_tft.setRotation(ConfigManager::getInstance()->getConfigInt("orbRotation", ORB_ROTATION));
    m_tft.fillScreen(TFT_WHITE);
    m_tft.setTextDatum(MC_DATUM);
    reset();

    // Init TJpg_Decode
    TJpgDec.setSwapBytes(true); // JPEG rendering setup
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(tftOutput);

    // I'm not sure which cache size is actually good.
    // It's a tradeoff between memory consumption and render speed.
    // Needs more testing to find the sweet spot.
    m_render.setCacheSize(8, 8, 4096);
    setFont(DEFAULT_FONT);
    m_render.setDrawer(m_tft);

    Serial.println("ScreenManager initialized");
    Serial.println("TFT_MOSI:" + String(TFT_MOSI));
    Serial.println("TFT_MISO:" + String(TFT_MISO));
    Serial.println("TFT_SCLK:" + String(TFT_SCLK));
    Serial.println("TFT_CS:" + String(TFT_CS));
    Serial.println("TFT_DC:" + String(TFT_DC));
    Serial.println("TFT_RST:" + String(TFT_RST));
    Serial.println("SCREEN_1_CS:" + String(SCREEN_1_CS));
    Serial.println("SCREEN_2_CS:" + String(SCREEN_2_CS));
    Serial.println("SCREEN_3_CS:" + String(SCREEN_3_CS));
    Serial.println("SCREEN_4_CS:" + String(SCREEN_4_CS));
    Serial.println("SCREEN_5_CS:" + String(SCREEN_5_CS));

    instance = this;
}

void ScreenManager::setFont(TTF_Font font) {
    if (font == m_curFont) {
        // nothing to do
        return;
    }
    m_render.unloadFont();
    // Font is now unloaded
    m_curFont = TTF_Font::NONE;
    if (font == TTF_Font::NONE) {
        // just unload
        return;
    }
    // 0 is success
    FT_Error error = 1;
    switch (font) {
    case ROBOTO_REGULAR:
        error = m_render.loadFont(robotoRegular_start, robotoRegular_end - robotoRegular_start);
        break;

    case FINAL_FRONTIER:
        error = m_render.loadFont(finalFrontier_start, finalFrontier_end - finalFrontier_start);
        break;

    case DSEG7:
        error = m_render.loadFont(dseg7_start, dseg7_end - dseg7_start);
        break;

    case DSEG14:
        error = m_render.loadFont(dseg14_start, dseg14_end - dseg14_start);
        break;
    }
    if (error == 0) {
        m_curFont = font;
    } else {
        Serial.printf("Unable to load TTF font %d\n", font);
    }
}

TFT_eSPI &ScreenManager::getDisplay() {
    return m_tft;
}

OpenFontRender &ScreenManager::getRender() {
    return m_render;
}

// Selects a single screen
void ScreenManager::selectScreen(int screen) {
    for (int i = 0; i < NUM_SCREENS; i++) {
        int orbRotation = ConfigManager::getInstance()->getConfigInt("orbRotation", ORB_ROTATION);
        bool rotateDisplays = orbRotation == 1 || orbRotation == 2;
        int currentDisplay = rotateDisplays ? NUM_SCREENS - i - 1 : i;
        digitalWrite(m_screen_cs[currentDisplay], i == screen ? LOW : HIGH);
    }
}

// Fills all screens with a color
void ScreenManager::fillAllScreens(uint32_t color) {
    selectAllScreens();
    fillScreen(color);
    reset();
}

// Clears all screens by resetting them to black
void ScreenManager::clearAllScreens() {
    fillAllScreens(TFT_BLACK);
}

// Clears one screens by resetting it to black
void ScreenManager::clearScreen(int screen) {
    if (screen >= 0) {
        selectScreen(screen);
    }
    fillScreen(TFT_BLACK);
}

void ScreenManager::fillScreen(uint32_t color) {
    m_tft.fillScreen(dim(color));
    // Set background for aliasing as well
    m_render.setBackgroundColor(dim(color));
}

bool ScreenManager::setBrightness(uint8_t brightness) {
    if (m_brightness != brightness) {
        Serial.printf("Brightness set to %d\n", brightness);
        m_brightness = brightness;
        return true;
    } else {
        return false;
    }
}

uint8_t ScreenManager::getBrightness() {
    return m_brightness;
}

void ScreenManager::setFontColor(uint32_t color) {
    m_render.setFontColor(dim(color));
}

void ScreenManager::setFontColor(uint32_t color, uint32_t background) {
    m_render.setFontColor(dim(color));
    m_render.setBackgroundColor(dim(background));
}

void ScreenManager::setBackgroundColor(uint32_t color) {
    m_render.setBackgroundColor(dim(color));
}

void ScreenManager::setAlignment(Align align) {
    m_render.setAlignment(align);
}

void ScreenManager::setFontSize(uint32_t size) {
    m_render.setFontSize(size);
}

// Selects all screens
// I don't think that state should be used, It's kinda weird saying "ow select
// all the screens to "off"
void ScreenManager::selectAllScreens() {
    for (int i = 0; i < NUM_SCREENS; i++) {
        digitalWrite(m_screen_cs[i], LOW);
    }
}

// Unselect all screens
void ScreenManager::reset() {
    for (int i = 0; i < NUM_SCREENS; i++) {
        digitalWrite(m_screen_cs[i], HIGH);
    }
}

unsigned int ScreenManager::calculateFitFontSize(uint32_t limit_width, uint32_t limit_height, Layout layout, const String &text) {
    unsigned int calcFontSize = m_render.calculateFitFontSize(limit_width, limit_height, layout, text.c_str());
    // Serial.printf("calcFitFontSize: t=%s, w=%d, h=%d -> fs=%d\n", str, limit_width, limit_height, calcFontSize);
    return calcFontSize;
}

void ScreenManager::drawString(const String &text, int x, int y) {
    // Use current font size and alignment
    drawString(text, x, y, 0, m_render.getAlignment());
}

void ScreenManager::drawString(const String &text, int x, int y, unsigned int fontSize, Align align, int32_t fgColor, int32_t bgColor, bool applyScale) {

    if (fontSize == 0) {
        // Keep current font size
        fontSize = m_render.getFontSize();
    } else if (applyScale) {
        fontSize = getScaledFontSize(fontSize);
    }
    if (fgColor == -1) {
        // Keep current FG color
        fgColor = m_render.getFontColor();
    } else {
        fgColor = dim(fgColor);
    }
    if (bgColor == -1) {
        // Keep current BG color
        bgColor = m_render.getBackgroundColor();
    } else {
        bgColor = dim(bgColor);
    }

    // Dirty hack to correct misaligned Y
    // See https://github.com/takkaO/OpenFontRender/issues/38
    FT_BBox box = m_render.calculateBoundingBox(0, 0, fontSize, Align::TopLeft, Layout::Horizontal, text.c_str());
    m_render.setAlignment(align);
    m_render.setFontSize(fontSize);
    m_render.drawString(text.c_str(), x, y - box.yMin, fgColor, bgColor);
}

void ScreenManager::drawCentreString(const String &text, int x, int y, unsigned int fontSize) {
    drawString(text, x, y, fontSize, Align::MiddleCenter);
}

void ScreenManager::drawFittedString(const String &text, int x, int y, int limit_w, int limit_h, Align align) {
    unsigned int fontSize = calculateFitFontSize(limit_w, limit_h, Layout::Horizontal, text);
    drawString(text, x, y, fontSize, align, -1, -1, false);
}

void ScreenManager::drawFittedString(const String &text, int x, int y, int limit_w, int limit_h) {
    drawFittedString(text, x, y, limit_w, limit_h, m_render.getAlignment());
}

void ScreenManager::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    m_tft.drawRect(x, y, w, h, dim(color));
}

void ScreenManager::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    m_tft.fillRect(x, y, w, h, dim(color));
}

void ScreenManager::drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color) {
    m_tft.drawLine(xs, ys, xe, ye, dim(color));
}

void ScreenManager::drawArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool smoothArc) {
    m_tft.drawArc(x, y, r, ir, startAngle, endAngle, dim(fg_color), dim(bg_color), smoothArc);
}

void ScreenManager::drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool roundEnds) {
    m_tft.drawSmoothArc(x, y, r, ir, startAngle, endAngle, dim(fg_color), dim(bg_color), roundEnds);
}

void ScreenManager::drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color) {
    m_tft.drawTriangle(x1, y1, x2, y2, x3, y3, dim(color));
}

void ScreenManager::fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color) {
    m_tft.fillTriangle(x1, y1, x2, y2, x3, y3, dim(color));
}

void ScreenManager::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    m_tft.drawCircle(x, y, r, dim(color));
}

void ScreenManager::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    m_tft.fillCircle(x, y, r, dim(color));
}

unsigned int ScreenManager::getScaledFontSize(unsigned int fontSize) {
    for (TTF_FontMetric metric : ttfFontMetrics) {
        if (metric.font == m_curFont) {
            return round(metric.scale * fontSize);
        }
    }
    return fontSize;
}

// get the dimmed color (using current brightness)
uint16_t ScreenManager::dim(uint16_t color) {
    return Utils::rgb565dim(color, m_brightness);
}

int16_t ScreenManager::getLegacyFontHeight() {
    return m_tft.fontHeight();
}

void ScreenManager::setLegacyTextColor(uint16_t color) {
    m_tft.setTextColor(dim(color));
}

void ScreenManager::setLegacyTextColor(uint16_t fgcolor, uint16_t bgcolor, bool bgfill) {
    m_tft.setTextColor(dim(fgcolor), dim(bgcolor), bgfill);
}

void ScreenManager::setLegacyTextDatum(uint8_t datum) {
    m_tft.setTextDatum(datum);
}

void ScreenManager::setLegacyTextSize(uint8_t size) {
    m_tft.setTextSize(size);
}

void ScreenManager::setLegacyTextFont(uint8_t font) {
    m_tft.setTextFont(font);
}

void ScreenManager::drawLegacyString(const String &string, int32_t x, int32_t y) {
    m_tft.drawString(string, x, y);
}

void ScreenManager::drawLegacyString(const String &string, int32_t x, int32_t y, uint8_t font) {
    m_tft.drawString(string, x, y, font);
}

int16_t ScreenManager::drawLegacyChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font) {
    return m_tft.drawChar(uniCode, x, y, font);
}

// Static function to be used in TJpgDec callback
bool ScreenManager::tftOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (instance == nullptr) {
        Serial.println("TFT_Output not possible, ScreenManager instance not initialized");
        return false;
    }
    uint8_t brightness = instance->getBrightness();
    uint32_t imageColor = instance->m_imageColor;
    TFT_eSPI &tft = instance->getDisplay();
    if (y >= tft.height() || x >= tft.width())
        return 0;
    if (imageColor != 0) {
        // We have an image color set, let's use it
        Utils::colorizeImageData(bitmap, w * h, imageColor, 1.25, true);
    }
    if (brightness != 255) {
        // Dim bitmap
        Utils::rgb565dimBitmap(bitmap, w * h, brightness, true);
    }
    tft.pushImage(x, y, w, h, bitmap);
    return true;
}

JRESULT ScreenManager::drawJpg(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t data_size, uint8_t scale, uint32_t imageColor) {
    // Set scale
    TJpgDec.setJpgScale(scale);
    // Set image color
    m_imageColor = imageColor;
    JRESULT result = TJpgDec.drawJpg(x, y, jpeg_data, data_size);
    // Reset image color
    m_imageColor = 0;
    return result;
}