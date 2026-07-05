#include "CalendarWidget.h"

#include "GlobalTime.h"
#include "Utils.h"

CalendarWidget::CalendarWidget(ScreenManager &manager, String icsUrl) : Widget(manager), m_icsUrl(icsUrl) {
}

void CalendarWidget::setup() {
    Serial.println("CalendarWidget: setup, url=" + m_icsUrl);
}

void CalendarWidget::networkRefreshIfDue(bool force) {
    GlobalTime *globalTime = GlobalTime::getInstance();
    int currentHour = globalTime->getHour24();

    if (!force && currentHour == m_lastFetchedHour) {
        return;
    }

    setBusy(true);
    bool ok = m_dataModel.fetchAndParse(m_icsUrl);
    setBusy(false);

    if (m_dataModel.getStatus() == CalendarFetchStatus::NOT_READY) {
        // GlobalTime hasn't produced a valid "now" yet (this can happen on
        // the very first update() call at boot - see the boot-sequencing
        // note in the plan). Do NOT advance m_lastFetchedHour, so the next
        // time update() runs (whenever that is) it tries again rather than
        // waiting up to an hour.
        return;
    }

    m_lastFetchedHour = currentHour;
    if (ok) {
        m_needsRedraw = true;
    }
}

void CalendarWidget::localReevaluateIfDue() {
    if (millis() - m_lastLocalEvalMillis < 600000UL) {
        return;
    }
    // Phase 1's draw() has no countdown/ongoing-ring rendering yet, so this
    // re-evaluation has minimal visible effect - it exists as the timing
    // scaffolding Phase 2's ongoing-event ring/countdown text plugs into.
    m_needsRedraw = true;
    m_lastLocalEvalMillis = millis();
}

void CalendarWidget::update(bool force) {
    networkRefreshIfDue(force);
    localReevaluateIfDue();
}

void CalendarWidget::draw(bool force) {
    if (!force && !m_needsRedraw) {
        return;
    }

    // Phase 1 placeholder: screen 0 only, plain text. Phase 2 replaces this
    // entire method with the 5-orb layout (month grid, agenda slots,
    // marquee, ongoing-event ring) built on the same CalendarDataModel.
    m_manager.selectScreen(0);
    m_manager.fillScreen(TFT_BLACK);
    m_manager.setFont(DEFAULT_FONT);
    m_manager.setFontColor(CLOCK_COLOR, TFT_BLACK);

    int count = m_dataModel.getEventCount();
    if (count == 0) {
        m_manager.drawCentreString("No events", ScreenCenterX, ScreenCenterY, 18);
    } else {
        int shown = count < 5 ? count : 5;
        int y = 40;
        for (int i = 0; i < shown; i++) {
            const CalendarEvent &event = m_dataModel.getEvent(i);
            m_manager.drawString(String(event.title), 10, y, 12, Align::MiddleLeft);
            y += 30;
        }
    }

    m_needsRedraw = false;
}

void CalendarWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    // Only BTN_LONG (manual network sync, per ADR decision 6) is meaningful
    // without real rendering. BTN_SHORT (page toggle) and BTN_MEDIUM (mode
    // toggle) are no-ops until Phase 2 introduces that state.
    if (buttonId == BUTTON_OK && state == BTN_LONG) {
        networkRefreshIfDue(true);
    }
}

String CalendarWidget::getName() {
    return "Calendar";
}
