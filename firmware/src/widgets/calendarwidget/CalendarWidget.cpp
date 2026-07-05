#include "CalendarWidget.h"

#include "Utils.h"
#include <TimeLib.h>
#include <string.h>

// Internal rendering-tuning constants (not user-facing config.h values).
const time_t CALENDAR_STALE_THRESHOLD_SECONDS = 7200; // ~2 missed hourly syncs

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
    String syncText;
    uint32_t syncColor = TFT_WHITE;
    if (m_lastSyncEpoch == 0) {
        syncText = "Syncing...";
    } else {
        syncText = "Synced " + formatEventTime(m_lastSyncEpoch);
        if (m_time->getUnixEpoch() - m_lastSyncEpoch > CALENDAR_STALE_THRESHOLD_SECONDS) {
            syncColor = TFT_DARKGREY; // several hourly syncs in a row have failed
        }
    }
    m_manager.setFontColor(syncColor, TFT_BLACK);
    m_manager.drawFittedString(syncText, 120, 214, 150, 18, Align::MiddleCenter);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK); // restore in case anything else draws on this screen later
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

String CalendarWidget::formatAgendaDayLabel(time_t eventTime, time_t now) {
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

// 34 was too optimistic - some titles/locations that should have wrapped
// were instead rendered as a single line wide enough to run off-screen.
// Back to a safer cap; width is gained via the smaller left margin instead.
const int kAgendaTextMaxCharsPerLine = 28;
// Title/location sit close to vertical center (y=80-152), where the circle
// is near its widest, so they can push further left than rows nearer the
// top/bottom of the orb.
const int kAgendaTitleLocationMargin = 15;
// Day-view event rows span a wider y-range (down to ~205), so they need the
// more conservative margin already proven safe for the month grid's gridLeft.
const int kAgendaLeftMargin = 35;

void CalendarWidget::splitTextForDisplay(const String &text, int maxCharsPerLine, String &line1, String &line2) {
    if ((int)text.length() <= maxCharsPerLine) {
        line1 = text;
        line2 = "";
        return;
    }

    // Search outward from the midpoint for a space to break on, so we don't
    // cut a word in half under normal conditions.
    int mid = text.length() / 2;
    int breakPos = -1;
    for (int offset = 0; offset <= mid; offset++) {
        if (mid - offset >= 0 && text.charAt(mid - offset) == ' ') {
            breakPos = mid - offset;
            break;
        }
        if (mid + offset < (int)text.length() && text.charAt(mid + offset) == ' ') {
            breakPos = mid + offset;
            break;
        }
    }
    if (breakPos == -1) {
        breakPos = maxCharsPerLine; // no space found - hard split
    }

    line1 = text.substring(0, breakPos);
    line1.trim();
    line2 = text.substring(breakPos);
    line2.trim();

    if ((int)line1.length() > maxCharsPerLine) {
        line1 = line1.substring(0, maxCharsPerLine - 3) + "...";
    }
    if ((int)line2.length() > maxCharsPerLine) {
        line2 = line2.substring(0, maxCharsPerLine - 3) + "...";
    }
}

void CalendarWidget::drawAgendaSlot(int screenIndex, const CalendarEvent *event, bool isOngoing,
                                     const String &emptyMessage) {
    const int calendarFontSize = 12; // matches the month-grid's date/weekday font size

    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    if (event == nullptr) {
        m_manager.drawFittedString(emptyMessage, 120, 120, 200, 40, Align::MiddleCenter);
        return;
    }

    time_t now = m_time->getUnixEpoch();
    String dayLabel = formatAgendaDayLabel((time_t)event->start, now);
    String duration = formatAgendaDuration(*event);
    String titleLine1, titleLine2;
    splitTextForDisplay(String(event->title), kAgendaTextMaxCharsPerLine, titleLine1, titleLine2);

    // Packed tightly top-to-bottom: day, duration, title (up to 2 lines),
    // location (up to 2 lines) - all six rows fit without the location
    // getting pushed toward the bezel edge like before. Day/duration stay
    // centered; title/location are left-aligned (and pushed further left)
    // since they're the parts most likely to need the extra width.
    m_manager.drawString(dayLabel, 120, 30, calendarFontSize, Align::MiddleCenter);
    m_manager.drawString(duration, 120, 52, calendarFontSize, Align::MiddleCenter);
    m_manager.setFont(ROBOTO_BOLD);
    m_manager.drawString(titleLine1, kAgendaTitleLocationMargin, 80, calendarFontSize, Align::MiddleLeft);
    if (titleLine2.length() > 0) {
        m_manager.drawString(titleLine2, kAgendaTitleLocationMargin, 102, calendarFontSize, Align::MiddleLeft);
    }
    m_manager.setFont(DEFAULT_FONT);

    if (strlen(event->location) > 0) {
        String locationLine1, locationLine2;
        splitTextForDisplay(String(event->location), kAgendaTextMaxCharsPerLine, locationLine1, locationLine2);
        m_manager.drawString(locationLine1, kAgendaTitleLocationMargin, 130, calendarFontSize, Align::MiddleLeft);
        if (locationLine2.length() > 0) {
            m_manager.drawString(locationLine2, kAgendaTitleLocationMargin, 152, calendarFontSize, Align::MiddleLeft);
        }
    } else {
        m_manager.drawString("No location set", kAgendaTitleLocationMargin, 130, calendarFontSize, Align::MiddleLeft);
    }

    // Ongoing-event ring: same thin drawArc-at-the-edge technique StockWidget
    // uses for its red/green price-direction ring (StockWidget.cpp:134,138),
    // just in orange for "happening now" instead.
    if (isOngoing) {
        m_manager.drawArc(120, 120, 120, 118, 0, 360, TFT_ORANGE, TFT_ORANGE);
    }
}

int CalendarWidget::collectEventsForDay(time_t dayStart, const CalendarEvent *outEvents[], int maxCount,
                                          int &totalCount) {
    totalCount = 0;
    int shown = 0;
    time_t dayEnd = dayStart + 86400;
    int total = m_dataModel.getEventCount();
    for (int i = 0; i < total; i++) {
        const CalendarEvent &event = m_dataModel.getEvent(i);
        // Overlaps this day if it starts before the day ends, and (for a
        // timed event) ends after the day starts - so a multi-day or
        // ongoing-from-yesterday event still shows up. A point-in-time
        // event (no DTEND) belongs to the day its start falls in.
        bool matches = event.start < (uint32_t)dayEnd &&
                       (event.end == 0 ? event.start >= (uint32_t)dayStart : event.end > (uint32_t)dayStart);
        if (matches) {
            totalCount++;
            if (shown < maxCount) {
                outEvents[shown++] = &event;
            }
        }
    }
    return shown;
}

void CalendarWidget::drawAgendaDaySlot(int screenIndex, time_t dayStart) {
    const int calendarFontSize = 12;

    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    time_t now = m_time->getUnixEpoch();
    String dayLabel = formatAgendaDayLabel(dayStart, now);
    m_manager.drawString(dayLabel, 120, 32, calendarFontSize, Align::MiddleCenter);

    const int maxDayEvents = 9; // bumped from 8 - one more fits within the same tight packing
    const CalendarEvent *dayEvents[maxDayEvents] = {nullptr, nullptr, nullptr, nullptr, nullptr,
                                                     nullptr, nullptr, nullptr, nullptr};
    int totalCount = 0;
    int shown = collectEventsForDay(dayStart, dayEvents, maxDayEvents, totalCount);

    if (shown == 0) {
        m_manager.drawString("No events", 120, 80, calendarFontSize, Align::MiddleCenter);
        return;
    }

    // Half a row's gap after the day label, then events packed back-to-back
    // with no blank line between them (just their own line height). Events
    // stay left-aligned (day label and "+more" stay centered).
    const int firstEventY = 52;
    const int eventRowSpacing = 17;
    const int maxTitleCharsCompact = 20; // shorter than the events-mode title cap - shares the row with a time prefix
    for (int i = 0; i < shown; i++) {
        String timePart = dayEvents[i]->allDay ? "All day" : formatEventTime((time_t)dayEvents[i]->start);
        String titlePart = String(dayEvents[i]->title);
        if ((int)titlePart.length() > maxTitleCharsCompact) {
            titlePart = titlePart.substring(0, maxTitleCharsCompact - 3) + "...";
        }
        int y = firstEventY + i * eventRowSpacing;
        String timePrefix = timePart + "  ";
        m_manager.drawString(timePrefix, kAgendaLeftMargin, y, calendarFontSize, Align::MiddleLeft);
        int titleX = kAgendaLeftMargin + m_manager.getTextWidth(timePrefix, calendarFontSize);
        m_manager.setFont(ROBOTO_BOLD);
        m_manager.drawString(titlePart, titleX, y, calendarFontSize, Align::MiddleLeft);
        m_manager.setFont(DEFAULT_FONT);
    }

    if (totalCount > shown) {
        int y = firstEventY + shown * eventRowSpacing;
        m_manager.drawString("+" + String(totalCount - shown) + " more", 120, y, calendarFontSize,
                              Align::MiddleCenter);
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
    // Re-derives what's "next"/ongoing from already-stored data - this is
    // what lets a finished event drop off (and the ongoing-event ring
    // appear/disappear) without waiting for the next hourly network fetch.
    m_needsRedraw = true;
    m_lastLocalEvalMillis = millis();
}

void CalendarWidget::toggleAgendaPage() {
    m_agendaPage = 1 - m_agendaPage;
    m_needsRedraw = true;
}

void CalendarWidget::changeAgendaMode() {
    m_agendaMode = (m_agendaMode == AgendaMode::EVENTS) ? AgendaMode::DAYS : AgendaMode::EVENTS;
    m_agendaPage = 0;
    m_needsRedraw = true;
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

        if (m_agendaMode == AgendaMode::EVENTS) {
            const CalendarEvent *nextEvents[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
            int found = findNextRelevantEvents(now, nextEvents, 6);
            int pageOffset = m_agendaPage * 3;

            for (int slot = 0; slot < 3; slot++) {
                int idx = pageOffset + slot;
                const CalendarEvent *event = (idx < found) ? nextEvents[idx] : nullptr;
                bool isOngoing = event && event->start <= (uint32_t)now &&
                                  (event->end == 0 || event->end > (uint32_t)now);
                drawAgendaSlot(2 + slot, event, isOngoing, "No more events in the next 6 days");
            }
        } else {
            time_t todayStart = previousMidnight(now);
            for (int slot = 0; slot < 3; slot++) {
                time_t dayStart = todayStart + (time_t)(m_agendaPage * 3 + slot) * 86400;
                drawAgendaDaySlot(2 + slot, dayStart);
            }
        }

        m_needsRedraw = false;
    }
}

void CalendarWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId != BUTTON_OK) {
        return;
    }
    if (state == BTN_SHORT) {
        toggleAgendaPage();
    } else if (state == BTN_MEDIUM) {
        changeAgendaMode();
    } else if (state == BTN_LONG) {
        networkRefreshIfDue(true);
    }
}

String CalendarWidget::getName() {
    return "Calendar";
}
