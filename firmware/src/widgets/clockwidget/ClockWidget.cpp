#include "ClockWidget.h"

ClockWidget::ClockWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config) {
    m_enabled = true; // Always enabled, do not add a config setting for it
    addConfigToManager();
}

ClockWidget::~ClockWidget() {
}

void ClockWidget::addConfigToManager() {
    String optClockType[2 + USE_CLOCK_CUSTOM] = {
        "Normal Clock",
        "Nixie Clock"};
    if (!USE_CLOCK_NIXIE)
        optClockType[(int) ClockType::NIXIE] += " (n/a)";
    for (int i = 0; i < USE_CLOCK_CUSTOM; i++) {
        optClockType[(int) ClockType::CUSTOM0 + i] = "Custom Clock " + String(i);
    }

    m_config.addConfigComboBox("ClockWidget", "defaultType", &m_type, optClockType, 2 + USE_CLOCK_CUSTOM, "Default Clock Type (you can also switch types with the middle button)");
    if (!isValidClockType(m_type)) {
        // Invalid Clock Type
        m_type = (int) ClockType::NORMAL;
    }
    String optFormats[] = {"24h mode", "12h mode", "12h mode (with am/pm)"};
    m_config.addConfigComboBox("ClockWidget", "clockFormat", &m_format, optFormats, 3, "Clock Format");
    m_config.addConfigBool("ClockWidget", "showSecondTicks", &m_showSecondTicks, "Show Second Ticks", true);
    m_config.addConfigColor("ClockWidget", "clkColor", &m_fgColor, "Clock Color", true);
    m_config.addConfigBool("ClockWidget", "clkShadowing", &m_shadowing, "Clock Shadowing", true);
    m_config.addConfigColor("ClockWidget", "clkShColor", &m_shadowColor, "Clock Shadow Color", true);
    m_config.addConfigColor("ClockWidget", "clkNixieColor", &m_overrideNixieColor, "Override Nixie color (black=disable)", true);

#if USE_CLOCK_CUSTOM > 0
    for (int i = 0; i < USE_CLOCK_CUSTOM; i++) {
        const char *tickKey = Utils::createConstCharBufferAndConcat("clkCust", String(i).c_str(), "tckCol");
        const char *tickDesc = Utils::createConstCharBufferAndConcat("CustomClock", String(i).c_str(), ": Second Tick Color");
        m_config.addConfigColor("ClockWidget", tickKey, &m_customTickColor[i], tickDesc, true);
        const char *overrideKey = Utils::createConstCharBufferAndConcat("clkCust", String(i).c_str(), "ovrCol");
        const char *overrideDesc = Utils::createConstCharBufferAndConcat("CustomClock", String(i).c_str(), ": Override color (black=disable)");
        m_config.addConfigColor("ClockWidget", overrideKey, &m_customOverrideColor[i], overrideDesc, true);
    }
#endif
}

void ClockWidget::setup() {
    m_lastDisplay1Digit = "";
    m_lastDisplay2Digit = "";
    m_lastDisplay4Digit = "";
    m_lastDisplay5Digit = "";
}

void ClockWidget::draw(bool force) {
    m_manager.setFont(CLOCK_FONT);
    GlobalTime *time = GlobalTime::getInstance();

    if (m_lastDisplay1Digit != m_display1Digit || force) {
        displayDigit(0, m_lastDisplay1Digit, m_display1Digit, m_fgColor);
        m_lastDisplay1Digit = m_display1Digit;
    }
    if (m_lastDisplay2Digit != m_display2Digit || force) {
        displayDigit(1, m_lastDisplay2Digit, m_display2Digit, m_fgColor);
        m_lastDisplay2Digit = m_display2Digit;
    }
    if (m_lastDisplay4Digit != m_display4Digit || force) {
        displayDigit(3, m_lastDisplay4Digit, m_display4Digit, m_fgColor);
        m_lastDisplay4Digit = m_display4Digit;
    }
    if (m_lastDisplay5Digit != m_display5Digit || force) {
        displayDigit(4, m_lastDisplay5Digit, m_display5Digit, m_fgColor);
        m_lastDisplay5Digit = m_display5Digit;
    }

    if (m_secondSingle != m_lastSecondSingle || force) {
        if (m_secondSingle % 2 == 0) {
            displayDigit(2, "", ":", m_fgColor, false);
        } else {
            displayDigit(2, "", ":", m_shadowColor, false);
        }
        if (m_showSecondTicks) {
            if (!isCustomClock(m_type)) {
                // not a custom clock -> clear background
                displaySeconds(2, m_lastSecondSingle, TFT_BLACK);
            }
            displaySeconds(2, m_secondSingle, m_fgColor);
        }
        m_lastSecondSingle = m_secondSingle;
        if (m_type == (int) ClockType::NORMAL) {
            if (m_format == CLOCK_FORMAT_12_HOUR_AMPM) {
                if (m_amPm != m_lastAmPm) {
                    // Clear old AM/PM
                    displayAmPm(m_lastAmPm, TFT_BLACK);
                    m_lastAmPm = m_amPm;
                }
                displayAmPm(m_amPm, m_fgColor);
            }
        }
    }
}

void ClockWidget::displayAmPm(String &amPm, uint32_t color) {
    m_manager.selectScreen(2);
    m_manager.setFontColor(color, TFT_BLACK);
    // Workaround for 12h AM/PM problem
    // The colon is slightly offset and that's a problem because to remove them, we paint over them
    // I think this is related to the TTF cache
    // The problem disappears if we reload the font here
    if (CLOCK_FONT == TTF_Font::DSEG7) {
        // We set a new font anyway
        m_manager.setFont(TTF_Font::DSEG14);
    } else {
        // Force reloading the font
        m_manager.setFont(TTF_Font::NONE);
        m_manager.setFont(CLOCK_FONT);
    }
    m_manager.drawString(amPm, SCREEN_SIZE / 5 * 4, SCREEN_SIZE / 2, 25, Align::MiddleCenter);
}

void ClockWidget::update(bool force) {
    if (millis() - m_secondTimerPrev < m_secondTimer && !force) {
        return;
    }

    GlobalTime *time = GlobalTime::getInstance();
    if (force) {
        time->updateTime(true);
    }

    m_hourSingle = time->getHour();
    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();
    m_amPm = time->isPM() ? "PM" : "AM";

    if (m_lastHourSingle != m_hourSingle || force) {
        if (m_hourSingle < 10) {
            if (m_format == CLOCK_FORMAT_24_HOUR) {
                m_display1Digit = "0";
            } else {
                m_display1Digit = " ";
            }
        } else {
            m_display1Digit = int(m_hourSingle / 10);
        }
        m_display2Digit = m_hourSingle % 10;

        m_lastHourSingle = m_hourSingle;
    }

    if (m_lastMinuteSingle != m_minuteSingle || force) {
        String currentMinutePadded = String(m_minuteSingle).length() == 1 ? "0" + String(m_minuteSingle) : String(m_minuteSingle);

        m_display4Digit = currentMinutePadded.substring(0, 1);
        m_display5Digit = currentMinutePadded.substring(1, 2);

        m_lastMinuteSingle = m_minuteSingle;
    }
}

void ClockWidget::changeFormat() {
    GlobalTime *time = GlobalTime::getInstance();
    m_format++;
    if (m_format > 2) {
        m_format = 0;
    }
    time->setFormat24Hour(m_format == CLOCK_FORMAT_24_HOUR);
    m_manager.clearAllScreens();
    update(true);
    draw(true);
}

bool ClockWidget::isCustomClock(int clockType) {
    return (clockType >= (int) ClockType::CUSTOM0 && clockType <= (int) ClockType::CUSTOM9);
}

bool ClockWidget::isValidClockType(int clockType) {
    if (clockType == (int) ClockType::NORMAL)
        return true; // Always enabled
    else if (clockType == (int) ClockType::NIXIE)
        return USE_CLOCK_NIXIE > 0;
    else if (isCustomClock(clockType))
        return USE_CLOCK_CUSTOM > clockType - (int) ClockType::CUSTOM0;
    else
        return false;
}

void ClockWidget::changeClockType() {
    m_type++;
    if (m_type >= CLOCK_TYPE_NUM) {
        m_type = 0;
    }
    if (!isValidClockType(m_type)) {
        // Call recursively until a valid clock type is found
        changeClockType();
    } else {
        m_manager.clearAllScreens();
        draw(true);
    }
}

void ClockWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        changeClockType();
    } else if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        changeFormat();
    }
}

DigitOffset ClockWidget::getOffsetForDigit(const String &digit) {
    if (digit.length() > 0) {
        char c = digit.charAt(0);
        if (c >= '0' && c <= '9') {
            // get digit offsets
            return m_digitOffsets[c - '0'];
        }
    }
    // not a valid digit
    return {0, 0};
}

void ClockWidget::displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color, bool shadowing) {
    uint32_t start = millis();
    if (m_type == (int) ClockType::NIXIE || isCustomClock(m_type)) {
        if (digit == ":" && color == m_shadowColor) {
            // Show colon off
            displayDigitImage(displayIndex, " ");
        } else {
            displayDigitImage(displayIndex, digit);
        }
    } else {
        // Normal clock
        int fontSize = CLOCK_FONT_SIZE;
        char c = digit.charAt(0);
        bool isDigit = c >= '0' && c <= '9' || c == ' ';
        int defaultX = SCREEN_SIZE / 2 + (isDigit ? CLOCK_OFFSET_X_DIGITS : CLOCK_OFFSET_X_COLON);
        int defaultY = SCREEN_SIZE / 2;
        DigitOffset digitOffset = getOffsetForDigit(digit);
        DigitOffset lastDigitOffset = getOffsetForDigit(lastDigit);
        m_manager.selectScreen(displayIndex);
        if (shadowing) {
            m_manager.setFontColor(m_shadowColor, TFT_BLACK);
            if (CLOCK_FONT == DSEG14) {
                // DSEG14 (from DSEGstended) uses # to fill all segments
                m_manager.drawString("#", defaultX, defaultY, fontSize, Align::MiddleCenter);
            } else if (CLOCK_FONT == DSEG7) {
                // DESG7 uses 8 to fill all segments
                m_manager.drawString("8", defaultX, defaultY, fontSize, Align::MiddleCenter);
            } else {
                // Other fonts can't be shadowed
                m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
                m_manager.drawString(lastDigit, defaultX + lastDigitOffset.x, defaultY + lastDigitOffset.y, fontSize, Align::MiddleCenter);
            }
        } else {
            m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
            m_manager.drawString(lastDigit, defaultX + lastDigitOffset.x, defaultY + lastDigitOffset.y, fontSize, Align::MiddleCenter);
        }
        m_manager.setFontColor(color, TFT_BLACK);
        m_manager.drawString(digit, defaultX + digitOffset.x, defaultY + digitOffset.y, fontSize, Align::MiddleCenter);
    }
    uint32_t end = millis();
#ifdef CLOCK_DEBUG
    Serial.printf("displayDigit(%s) took %dms\n", digit, end - start);
#endif
}

void ClockWidget::displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color) {
    displayDigit(displayIndex, lastDigit, digit, color, m_shadowing);
}

void ClockWidget::displaySeconds(int displayIndex, int seconds, int color) {
    if (color != m_fgColor && m_type != (int) ClockType::NORMAL) {
        // ignore clear tick (we draw the whole image anyway)
        return;
    }
    if (m_type == (int) ClockType::NIXIE) {
        if (m_overrideNixieColor != TFT_BLACK) {
            // Selected override color for nixie
            color = m_overrideNixieColor;
        } else {
            // Special color (orange) for nixie
            color = 0xfd40;
        }
    } else if (isCustomClock(m_type)) {
        String tickColorKey = "clkCust" + String(m_type - (int) ClockType::CUSTOM0) + "tckCol";
        color = m_config.getConfigInt(tickColorKey.c_str(), TFT_WHITE);
    }
    m_manager.selectScreen(displayIndex);
    int startA = (seconds * 6) + 180 - 3;
    int endA = (seconds * 6) + 180 + 3;
    if (startA > 360) {startA = startA - 360;}
    if (endA > 360 ) {endA = endA - 360;}
    m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 100, startA, endA, color, TFT_BLACK);
}

void ClockWidget::displayDigitImage(int displayIndex, const String &digit) {
    if (digit.length() != 1) {
        return;
    }
    char c = digit.charAt(0);
    uint8_t index;
    if (c == ' ')
        index = 10;
    else if (c == ':')
        index = 11;
    else
        index = c - '0';
    if (index >= 12) {
        return;
    }
    if (m_type == (int) ClockType::NIXIE) {
        displayNixie(displayIndex, index);
    } else if (isCustomClock(m_type)) {
        displayCustom(displayIndex, m_type - (int) ClockType::CUSTOM0, index);
    }
}

void ClockWidget::displayNixie(int displayIndex, uint8_t index) {
#if USE_CLOCK_NIXIE > 0
    displayClockGraphics(displayIndex, clock_nixie, index, m_overrideNixieColor);
#endif
}

void ClockWidget::displayCustom(int displayIndex, uint8_t clockNumber, uint8_t index) {
#if USE_CLOCK_CUSTOM > 0
    String ovrColorKey = "clkCust" + String(clockNumber) + "ovrCol";
    int ovrColor = m_config.getConfigInt(ovrColorKey.c_str(), TFT_BLACK);
    m_manager.selectScreen(displayIndex);
    String name = "/CustomClock" + String(clockNumber) + "/" + String(index) + ".jpg";
    m_manager.drawFsJpg(0, 0, name.c_str(), 1, ovrColor);
#endif
}

void ClockWidget::displayClockGraphics(int displayIndex, const byte *clockArray[12][2], uint8_t index, int colorOverride) {
    m_manager.selectScreen(displayIndex);
    const byte *start = clockArray[index][0];
    const byte *end = clockArray[index][1];
    m_manager.drawJpg(0, 0, start, end - start, 1, colorOverride);
}

String ClockWidget::getName() {
    return "Clock";
}
