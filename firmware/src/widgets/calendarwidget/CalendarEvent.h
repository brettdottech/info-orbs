#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include "config_helper.h"
#include <Arduino.h>

// Fixed-size event record. Deliberately no String fields - the whole
// CalendarDataModel array is repopulated on every fetch (as often as
// hourly), and Arduino String heap allocations for that many long-lived
// entries would risk heap fragmentation over time.
struct CalendarEvent {
    uint32_t start = 0; // local-epoch seconds (same space as GlobalTime::getUnixEpoch())
    uint32_t end = 0; // local-epoch seconds, 0 if no DTEND was present
    bool allDay = false;
    char title[CALENDAR_TITLE_MAX_LEN + 1] = {0}; // null-terminated, truncated with "..." if longer
    char location[CALENDAR_LOCATION_MAX_LEN + 1] = {0}; // null-terminated, empty if no LOCATION was present

    bool isValid() const { return start != 0; }
};

#endif // CALENDAR_EVENT_H
