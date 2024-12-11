#include "ClockWidget.h"

#include "config_helper.h"

ClockWidget::ClockWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config) {
    m_enabled = true; // Always enabled, do not add a config setting for it
    String optClockType[] = {"Normal Clock", "Nixie Clock", "Custom Clock"};
    if (!USE_CLOCK_NIXIE)
        optClockType[(int) ClockType::NIXIE] += " (not available)";
    if (!USE_CLOCK_CUSTOM)
        optClockType[(int) ClockType::CUSTOM] += " (not available)";
    m_config.addConfigComboBox("ClockWidget", "defaultType", &m_type, optClockType, 3, "Default Clock Type (you can also switch types with the middle button)");
    if (m_type == (int) ClockType::NIXIE && !USE_CLOCK_NIXIE || m_type == (int) ClockType::CUSTOM && !USE_CLOCK_CUSTOM) {
        // Invalid Clock Type
        m_type = (int) ClockType::NORMAL;
    }
    String optFormats[] = {"24h mode", "12h mode", "12h mode (with am/pm)"};
    m_config.addConfigComboBox("ClockWidget", "clockFormat", &m_format, optFormats, 3, "Clock Format");
    m_config.addConfigBool("ClockWidget", "showSecondTicks", &m_showSecondTicks, "Show Second Ticks");
    m_config.addConfigColor("ClockWidget", "clkColor", &m_fgColor, "Clock Color");
    m_config.addConfigBool("ClockWidget", "clkShadowing", &m_shadowing, "Clock Shadowing");
    m_config.addConfigColor("ClockWidget", "clkShColor", &m_shadowColor, "Clock Shadow Color");
    m_config.addConfigBool("ClockWidget", "clkOvrNixCol", &m_overrideNixieColorEnabled, "Override Nixie Color");
    m_config.addConfigColor("ClockWidget", "clkNixieColor", &m_overrideNixieColor, "New Nixie color");
}

ClockWidget::~ClockWidget() {
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
            displaySeconds(2, m_lastSecondSingle, TFT_BLACK);
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

void ClockWidget::changeClockType() {
    switch (m_type) {
    case (int) ClockType::NORMAL:
        if (USE_CLOCK_NIXIE) {
            // If nixie is enabled, use it, otherwise fall through
            m_type = (int) ClockType::NIXIE;
            break;
        }

    case (int) ClockType::NIXIE:
        if (USE_CLOCK_CUSTOM) {
            // If custom is enabled, use it, otherwise fall through
            m_type = (int) ClockType::CUSTOM;
            break;
        }

    default:
        m_type = (int) ClockType::NORMAL;
        break;
    }
    m_manager.clearAllScreens();
    draw(true);
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
    if (m_type == (int) ClockType::NIXIE || m_type == (int) ClockType::CUSTOM) {
        if (digit == ":" && color == m_shadowColor) {
            // Show colon off
            displayImage(displayIndex, " ");
        } else {
            displayImage(displayIndex, digit);
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
}

void ClockWidget::displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color) {
    displayDigit(displayIndex, lastDigit, digit, color, m_shadowing);
}

void ClockWidget::displaySeconds(int displayIndex, int seconds, int color) {
    if (m_type == (int) ClockType::NIXIE && color == m_fgColor) {
        if (m_overrideNixieColorEnabled) {
            // Selected color for nixie
            color = m_overrideNixieColor;
        } else {
            // Special color (orange) for nixie
            color = 0xfd40;
        }
    }
    m_manager.selectScreen(displayIndex);
    if (seconds < 30) {
        m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds + 180, 6 * seconds + 180 + 6, color, TFT_BLACK);
    } else {
        m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds - 180, 6 * seconds - 180 + 6, color, TFT_BLACK);
    }
}

void ClockWidget::displayImage(int displayIndex, const String &digit) {
    switch (m_type) {
    case (int) ClockType::NIXIE:
        displayNixie(displayIndex, digit);
        break;

    case (int) ClockType::CUSTOM:
        displayCustom(displayIndex, digit);
        break;
    }
}

void ClockWidget::displayNixie(int displayIndex, const String &digit) {
#if USE_CLOCK_NIXIE
    int color = 0;
    if (m_overrideNixieColorEnabled) {
        color = m_overrideNixieColor;
    }
    if (digit.length() != 1) {
        return;
    }
    m_manager.selectScreen(displayIndex);
    switch (digit.charAt(0)) {
    case '0':
        m_manager.drawJpg(0, 0, nixie_0_start, nixie_0_end - nixie_0_start, 1, color);
        break;
    case '1':
        m_manager.drawJpg(0, 0, nixie_1_start, nixie_1_end - nixie_1_start, 1, color);
        break;
    case '2':
        m_manager.drawJpg(0, 0, nixie_2_start, nixie_2_end - nixie_2_start, 1, color);
        break;
    case '3':
        m_manager.drawJpg(0, 0, nixie_3_start, nixie_3_end - nixie_3_start, 1, color);
        break;
    case '4':
        m_manager.drawJpg(0, 0, nixie_4_start, nixie_4_end - nixie_4_start, 1, color);
        break;
    case '5':
        m_manager.drawJpg(0, 0, nixie_5_start, nixie_5_end - nixie_5_start, 1, color);
        break;
    case '6':
        m_manager.drawJpg(0, 0, nixie_6_start, nixie_6_end - nixie_6_start, 1, color);
        break;
    case '7':
        m_manager.drawJpg(0, 0, nixie_7_start, nixie_7_end - nixie_7_start, 1, color);
        break;
    case '8':
        m_manager.drawJpg(0, 0, nixie_8_start, nixie_8_end - nixie_8_start, 1, color);
        break;
    case '9':
        m_manager.drawJpg(0, 0, nixie_9_start, nixie_9_end - nixie_9_start, 1, color);
        break;
    case ' ':
        m_manager.drawJpg(0, 0, nixie_colon_off_start, nixie_colon_off_end - nixie_colon_off_start, 1, color);
        break;
    case ':':
        m_manager.drawJpg(0, 0, nixie_colon_on_start, nixie_colon_on_end - nixie_colon_on_start, 1, color);
        break;
    }
#endif
}

void ClockWidget::displayCustom(int displayIndex, const String &digit) {
#if USE_CLOCK_CUSTOM
    if (digit.length() != 1) {
        return;
    }
    m_manager.selectScreen(displayIndex);
    switch (digit.charAt(0)) {
    case '0':
        m_manager.drawJpg(0, 0, clock_custom_0_start, clock_custom_0_end - clock_custom_0_start);
        break;
    case '1':
        m_manager.drawJpg(0, 0, clock_custom_1_start, clock_custom_1_end - clock_custom_1_start);
        break;
    case '2':
        m_manager.drawJpg(0, 0, clock_custom_2_start, clock_custom_2_end - clock_custom_2_start);
        break;
    case '3':
        m_manager.drawJpg(0, 0, clock_custom_3_start, clock_custom_3_end - clock_custom_3_start);
        break;
    case '4':
        m_manager.drawJpg(0, 0, clock_custom_4_start, clock_custom_4_end - clock_custom_4_start);
        break;
    case '5':
        m_manager.drawJpg(0, 0, clock_custom_5_start, clock_custom_5_end - clock_custom_5_start);
        break;
    case '6':
        m_manager.drawJpg(0, 0, clock_custom_6_start, clock_custom_6_end - clock_custom_6_start);
        break;
    case '7':
        m_manager.drawJpg(0, 0, clock_custom_7_start, clock_custom_7_end - clock_custom_7_start);
        break;
    case '8':
        m_manager.drawJpg(0, 0, clock_custom_8_start, clock_custom_8_end - clock_custom_8_start);
        break;
    case '9':
        m_manager.drawJpg(0, 0, clock_custom_9_start, clock_custom_9_end - clock_custom_9_start);
        break;
    case ' ':
        m_manager.drawJpg(0, 0, clock_custom_colon_off_start, clock_custom_colon_off_end - clock_custom_colon_off_start);
        break;
    case ':':
        m_manager.drawJpg(0, 0, clock_custom_colon_on_start, clock_custom_colon_on_end - clock_custom_colon_on_start);
        break;
    }
#endif
}

String ClockWidget::getName() {
    return "Clock";
}