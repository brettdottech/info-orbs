#include "CalendarWidget.h"

#include "Utils.h"
#include <TimeLib.h>
#include <string.h>

CalendarWidget::CalendarWidget(ScreenManager &manager, String icsUrl) : Widget(manager), m_icsUrl(icsUrl) {
}

void CalendarWidget::setup() {
    m_time = GlobalTime::getInstance();
    Serial.println("CalendarWidget: setup, url=" + m_icsUrl);
}

void CalendarWidget::drawClock(int screenIndex) {
    const int clockY = 120, dayOfWeekY = 190, dateY = 50, centre = 120;

    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    m_manager.drawCentreString(m_time->getDayAndMonth(), centre, dateY, 18);
    m_manager.drawCentreString(m_time->getWeekday(), centre, dayOfWeekY, 22);

    m_manager.drawString(m_time->getHourPadded(), centre - 10, clockY, 66, Align::MiddleRight);
    m_manager.drawString(":", centre, clockY, 66, Align::MiddleCenter);
    m_manager.drawString(m_time->getMinutePadded(), centre + 10, clockY, 66, Align::MiddleLeft);
}

int CalendarWidget::getClockStamp() {
    return m_time->getHour() * 60 + m_time->getMinute();
}

int CalendarWidget::daysInMonth(int year, int month) {
    // Build day 1 of *next* month, subtract one day, read day() of the result.
    int nextMonth = month + 1;
    int nextYear = year;
    if (nextMonth > 12) {
        nextMonth = 1;
        nextYear++;
    }

    tmElements_t tm;
    tm.Year = nextYear - 1970;
    tm.Month = nextMonth;
    tm.Day = 1;
    tm.Hour = 0;
    tm.Minute = 0;
    tm.Second = 0;
    time_t firstOfNextMonth = makeTime(tm);
    return day(firstOfNextMonth - 86400);
}

int CalendarWidget::firstWeekdayOfMonth(int year, int month) {
    tmElements_t tm;
    tm.Year = year - 1970;
    tm.Month = month;
    tm.Day = 1;
    tm.Hour = 0;
    tm.Minute = 0;
    tm.Second = 0;
    time_t firstOfMonth = makeTime(tm);

    int sundayIndexed = weekday(firstOfMonth) - 1; // 0=Sunday..6=Saturday (codebase's existing convention)
    return (sundayIndexed + 6) % 7; // translate to 0=Monday..6=Sunday
}

void CalendarWidget::drawMonthGrid(int screenIndex) {
    const int calendarFontSize = 12; // same size used for weekday letters, dates, and the month/year label

    static const char *kWeekdayLetters[7] = {"M", "T", "W", "T", "F", "S", "S"};
    const int gridLeft = 35;
    const int headerY = 60; // shifted down half a row (was 48) to give the month/year label more room above
    const int cellSize = 24;
    const int firstRowY = 84; // shifted down half a row (was 72)
    const int todayRadius = 11;

    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    int year = m_time->getYear();
    int month = m_time->getMonth();
    int today = m_time->getDay();

    // MM/YYYY instead of the spelled-out month name - a fixed-width format
    // that doesn't grow/shrink with the calendar's locale/month ("July" vs
    // "September"), rendered at the same font size as the rest of the grid.
    String monthYearLabel = (month < 10 ? "0" + String(month) : String(month)) + "/" + String(year);
    m_manager.drawCentreString(monthYearLabel, 120, 30, calendarFontSize);

    for (int c = 0; c < 7; c++) {
        int x = gridLeft + c * cellSize + cellSize / 2;
        m_manager.drawString(kWeekdayLetters[c], x, headerY, calendarFontSize, Align::MiddleCenter);
    }
    int startCol = firstWeekdayOfMonth(year, month);
    int totalDays = daysInMonth(year, month);

    int dayNum = 1;
    for (int cell = 0; cell < 42 && dayNum <= totalDays; cell++) {
        if (cell < startCol) {
            continue; // leading blank cells before day 1
        }
        int col = cell % 7;
        int row = cell / 7;
        int x = gridLeft + col * cellSize + cellSize / 2;
        int y = firstRowY + row * cellSize;

        if (dayNum == today) {
            m_manager.fillCircle(x, y, todayRadius, TFT_WHITE);
            m_manager.drawString(String(dayNum), x, y, calendarFontSize, Align::MiddleCenter, TFT_BLACK, TFT_WHITE);
        } else {
            m_manager.drawString(String(dayNum), x, y, calendarFontSize, Align::MiddleCenter);
        }
        dayNum++;
    }

    // Nudged down slightly to keep clearance from the last date row, which
    // also moved down with the half-row shift above.
    String syncText = (m_lastSyncEpoch == 0) ? "Syncing..." : ("Synced " + formatEventTime(m_lastSyncEpoch));
    m_manager.drawFittedString(syncText, 120, 214, 150, 18, Align::MiddleCenter);
}

int CalendarWidget::findNextRelevantEvents(time_t now, const CalendarEvent *outEvents[], int maxCount) {
    int found = 0;
    int total = m_dataModel.getEventCount();
    for (int i = 0; i < total && found < maxCount; i++) {
        const CalendarEvent &event = m_dataModel.getEvent(i);
        bool relevant = (event.end > 0) ? (event.end > (uint32_t)now) : (event.start >= (uint32_t)now);
        if (relevant) {
            outEvents[found++] = &event;
        }
    }
    return found;
}

String CalendarWidget::formatEventTime(time_t t) {
    // Respects the globally-configured 12h/24h preference (same flag
    // ClockWidget/GlobalTime use), rather than hardcoding one format.
    if (m_time->getFormat24Hour()) {
        int h = hour(t);
        int m = minute(t);
        return (h < 10 ? "0" + String(h) : String(h)) + ":" + (m < 10 ? "0" + String(m) : String(m));
    }
    int h = hourFormat12(t);
    int m = minute(t);
    bool pm = isPM(t);
    return String(h) + ":" + (m < 10 ? "0" + String(m) : String(m)) + (pm ? " PM" : " AM");
}

String CalendarWidget::formatAgendaDayLabel(const CalendarEvent &event, time_t now) {
    time_t eventTime = (time_t)event.start;
    bool sameDay = (day(now) == day(eventTime) && month(now) == month(eventTime) && year(now) == year(eventTime));
    if (sameDay) {
        return "Today";
    }

    time_t tomorrowStart = previousMidnight(now) + 86400;
    time_t dayAfterTomorrowStart = tomorrowStart + 86400;
    if (eventTime >= tomorrowStart && eventTime < dayAfterTomorrowStart) {
        return "Tomorrow";
    }

    return String(LOC_MONTH[month(eventTime) - 1]).substring(0, 3) + " " + String(day(eventTime));
}

String CalendarWidget::formatAgendaDuration(const CalendarEvent &event) {
    if (event.allDay) {
        return "All day";
    }
    String timeStr = formatEventTime((time_t)event.start);
    if (event.end > 0 && event.end > event.start) {
        // Show the range so duration is implicit (e.g. "9:00 - 10:00").
        timeStr += " - " + formatEventTime((time_t)event.end);
    }
    return timeStr;
}

void CalendarWidget::drawAgendaSlot(int screenIndex, const CalendarEvent *event) {
    const int calendarFontSize = 12; // matches the month-grid's date/weekday font size

    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    if (event == nullptr) {
        // Phase 2a: simple placeholder for an empty slot. The fuller "No
        // events" vs "No more events in the next 6 days" distinction is
        // Phase 2b.
        m_manager.drawString("--", 120, 120, 18, Align::MiddleCenter);
        return;
    }

    time_t now = m_time->getUnixEpoch();
    String dayLabel = formatAgendaDayLabel(*event, now);
    String duration = formatAgendaDuration(*event);
    String title = String(event->title);

    // Day, then duration, then the title (rendered larger for emphasis -
    // this font set has no true bold weight, see note below), then
    // location - all four in that order per request.
    m_manager.drawString(dayLabel, 120, 55, calendarFontSize, Align::MiddleCenter);
    m_manager.drawString(duration, 120, 82, calendarFontSize, Align::MiddleCenter);
    m_manager.drawFittedString(title, 120, 128, 200, 50, Align::MiddleCenter);

    if (strlen(event->location) > 0) {
        m_manager.drawString(String(event->location), 120, 185, calendarFontSize, Align::MiddleCenter);
    }
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
        m_lastSyncEpoch = globalTime->getUnixEpoch();
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
    m_manager.setFont(DEFAULT_FONT);
    m_time->updateTime();

    // Orb 1 (clock): redraws every minute, independent of the rest of the
    // layout - mirrors WeatherWidget::draw()'s clockStamp pattern.
    int clockStamp = getClockStamp();
    if (force || clockStamp != m_lastClockStamp) {
        drawClock(0);
        m_lastClockStamp = clockStamp;
    }

    // Orb 2 (month grid) + orbs 3-5 (agenda): only redrawn when something
    // actually changed (hourly fetch / 10-min local re-evaluation), same
    // gating Phase 1 already used for the whole widget.
    if (force || m_needsRedraw) {
        drawMonthGrid(1);

        time_t now = m_time->getUnixEpoch();
        const CalendarEvent *nextEvents[3] = {nullptr, nullptr, nullptr};
        int found = findNextRelevantEvents(now, nextEvents, 3);

        for (int slot = 0; slot < 3; slot++) {
            drawAgendaSlot(2 + slot, slot < found ? nextEvents[slot] : nullptr);
        }

        m_needsRedraw = false;
    }
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
