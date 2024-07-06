#include "widgets/clockWidget.h"

#include <config.h>
#include <globalTime.h>

ClockWidget::ClockWidget(ScreenManager& manager) : Widget(manager) {
}

ClockWidget::~ClockWidget() {
}

void ClockWidget::setup() {
    m_lastDisplay1Didget = "-1";
    m_lastDisplay2Didget = "-1";
    m_lastDisplay4Didget = "-1";
    m_lastDisplay5Didget = "-1";
}

void ClockWidget::draw(bool force) {
    if (m_lastDisplay1Didget != m_display1Didget || force) {
        displayDidget(0, m_display1Didget, 7, 5, FOREGROUND_COLOR);
        m_lastDisplay1Didget = m_display1Didget;
        if (SHADOWING != 1 &&m_display1Didget == " ") {
            m_manager.clearScreen(0);
        }
    }
    if (m_lastDisplay2Didget != m_display2Didget || force) {
        displayDidget(1, m_display2Didget, 7, 5, FOREGROUND_COLOR);
        m_lastDisplay2Didget = m_display2Didget;
    }
    if (m_lastDisplay4Didget != m_display4Didget || force) {
        displayDidget(3, m_display4Didget, 7, 5, FOREGROUND_COLOR);
        m_lastDisplay4Didget = m_display4Didget;
    }
    if (m_lastDisplay5Didget != m_display5Didget || force) {
        displayDidget(4, m_display5Didget, 7, 5, FOREGROUND_COLOR);
        m_lastDisplay5Didget = m_display5Didget;
    }

    if (m_secondSingle != m_lastSecondSingle || force) {
        if (m_secondSingle % 2 == 0) {
            displayDidget(2, ":", 7, 5, FOREGROUND_COLOR, false);
        } else {
            displayDidget(2, ":", 7, 5, BG_COLOR, false);
        }
        m_lastSecondSingle = m_secondSingle;
        if (!FORMAT_24_HOUR && SHOW_AM_PM_INDICATOR) {
            displayAmPm(FOREGROUND_COLOR);
        }
    }
}

void ClockWidget::displayAmPm(uint32_t color) {
    GlobalTime* time = GlobalTime::getInstance();
    m_manager.selectScreen(2);
    TFT_eSPI& display = m_manager.getDisplay();
    display.setTextSize(4);
    display.setTextColor(color, TFT_BLACK, true);
    String am_pm = time->isPM() ? "PM" : "AM";
    display.drawString(am_pm, SCREEN_SIZE - 50, SCREEN_SIZE / 2, 1);
}

void ClockWidget::update(bool force) {
    if (millis() - m_secondTimerPrev < m_secondTimer && !force) {
        return;
    }

    GlobalTime* time = GlobalTime::getInstance();
    m_hourSingle = time->getHour();

    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();

    if (m_lastHourSingle != m_hourSingle || force) {
        if (m_hourSingle < 10) {
            if (FORMAT_24_HOUR) {
                m_display1Didget = "0";
            } else {
                m_display1Didget = " ";
            }
        } else {
            m_display1Didget = int(m_hourSingle/10);
        }
        m_display2Didget = m_hourSingle % 10;

        m_lastHourSingle = m_hourSingle;
    }

    if (m_lastMinuteSingle != m_minuteSingle || force) {
        String currentMinutePadded = String(m_minuteSingle).length() == 1 ? "0" + String(m_minuteSingle) : String(m_minuteSingle);

        m_display4Didget = currentMinutePadded.substring(0, 1);
        m_display5Didget = currentMinutePadded.substring(1, 2);

        m_lastMinuteSingle = m_minuteSingle;
    }
}

void ClockWidget::changeMode() {}

void ClockWidget::displayDidget(int displayIndex, const String& didget, int font, int fontSize, uint32_t color, bool shadowing) {
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& display = m_manager.getDisplay();
    display.setTextSize(fontSize);
    if (shadowing && font == 7) {
        display.setTextColor(BG_COLOR, TFT_BLACK);
        display.drawString("8", SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
        display.setTextColor(color);
    } else {
        display.setTextColor(color, TFT_BLACK);
    }
    display.drawString(didget, SCREEN_SIZE / 2, SCREEN_SIZE / 2, font);
}

void ClockWidget::displayDidget(int displayIndex, const String& didget, int font, int fontSize, uint32_t color) {
    this->displayDidget(displayIndex, didget, font, fontSize, color, SHADOWING);
}

void ClockWidget::displaySeconds(int displayIndex, int seconds, int color) {
    m_manager.reset();
    m_manager.selectScreen(displayIndex);
    TFT_eSPI& display = m_manager.getDisplay();
    if (seconds < 30) {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds + 180, 6 * seconds + 180 + 6, color, TFT_BLACK);
    } else {
        display.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds - 180, 6 * seconds - 180 + 6, color, TFT_BLACK);
    }
}
