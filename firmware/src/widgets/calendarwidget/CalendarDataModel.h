#ifndef CALENDAR_DATA_MODEL_H
#define CALENDAR_DATA_MODEL_H

#include "CalendarEvent.h"
#include "config_helper.h"
#include <Arduino.h>

enum class CalendarFetchStatus {
    NOT_YET_FETCHED,
    OK,
    HTTP_ERROR,
    NOT_READY // clock/timezone not yet known - see CalendarWidget's boot-sequencing guard
};

// Owns the fetch->parse->store pipeline for one ICS feed. The event array is
// only ever replaced wholesale, and only on a successful fetch+parse - on
// failure the previous (last-good) list is left untouched, matching
// StockWidget's "skip on error, don't blank the display" behavior.
class CalendarDataModel {
public:
    CalendarDataModel();

    // Fetches `url`, parses events with a DTSTART in
    // [now, now + CALENDAR_WINDOW_DAYS days] (now/offset read from
    // GlobalTime), and replaces the stored event list on success.
    // Returns true if the fetch+parse produced a usable (possibly empty)
    // event list.
    bool fetchAndParse(const String &url);

    int getEventCount() const;
    const CalendarEvent &getEvent(int index) const;

    CalendarFetchStatus getStatus() const;

private:
    CalendarEvent m_events[CALENDAR_MAX_EVENTS];
    int m_eventCount = 0;
    CalendarFetchStatus m_status = CalendarFetchStatus::NOT_YET_FETCHED;

    void sortEventsByStart();
};

#endif // CALENDAR_DATA_MODEL_H
