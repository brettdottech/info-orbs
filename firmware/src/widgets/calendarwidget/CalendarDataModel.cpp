#include "CalendarDataModel.h"

#include "GlobalTime.h"
#include "IcsParser.h"
#include <HTTPClient.h>
#include <StreamUtils.h>

namespace {
// Sanity floor for "GlobalTime has produced a real now" (~2023-11-14).
// Anything before this means GlobalTime::updateTime() hasn't run yet (e.g.
// this is the very first update() call at boot, which can happen before
// GlobalTime ever ticks once - see CalendarWidget's boot-sequencing guard).
const time_t CLOCK_READY_THRESHOLD = 1700000000;
} // namespace

CalendarDataModel::CalendarDataModel() {
}

bool CalendarDataModel::fetchAndParse(const String &url) {
    GlobalTime *globalTime = GlobalTime::getInstance();
    time_t now = globalTime->getUnixEpoch();
    if (now < CLOCK_READY_THRESHOLD) {
        // A 6-day window computed against a bogus epoch would reject every
        // real event. Skip this attempt without touching m_events/m_status
        // beyond flagging NOT_READY - the caller must not treat this as a
        // completed attempt, so it retries the next time it runs.
        m_status = CalendarFetchStatus::NOT_READY;
        return false;
    }

    int offsetSeconds = globalTime->getTimeZoneOffsetSeconds();
    if (offsetSeconds == -1) {
        // Timezone offset not yet fetched from the API. Falling back to 0
        // here would parse every Z-suffixed/TZID timestamp as if the device
        // were in UTC, silently misplacing every event by the device's real
        // UTC offset until the next hourly re-fetch. Treat this the same as
        // the epoch-not-ready case above instead: skip this attempt (no
        // HTTP fetch spent) and flag NOT_READY, so networkRefreshIfDue()
        // doesn't advance m_lastFetchedHour and retries on its very next
        // update() call rather than waiting up to an hour with a
        // wrong-offset parse - this is expected to resolve within a second
        // or two after boot, once GlobalTime's own throttled API call
        // returns.
        m_status = CalendarFetchStatus::NOT_READY;
        return false;
    }

    time_t windowStart = now;
    time_t windowEnd = now + (time_t)CALENDAR_WINDOW_DAYS * 86400;

    HTTPClient http;
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setRedirectLimit(3);
    const char *keys[] = {"Transfer-Encoding"};
    http.collectHeaders(keys, 1);
    http.begin(url);

    int httpCode = http.GET();
    Serial.printf("Calendar: HTTP %d, Size %d\n", httpCode, http.getSize());

    if (httpCode != 200) {
        Serial.printf("Calendar: HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        m_status = CalendarFetchStatus::HTTP_ERROR;
        return false; // keep whatever event list we already had (last-good)
    }

    Stream &rawStream = http.getStream();
    ChunkDecodingStream decodedStream(http.getStream());
    // Some ICS servers send chunked responses.
    Stream &response = http.header("Transfer-Encoding") == "chunked" ? decodedStream : rawStream;

    IcsParser parser;
    int count = parser.parse(response, windowStart, windowEnd, offsetSeconds, m_events, CALENDAR_MAX_EVENTS);

    http.end();

    m_eventCount = count;
    sortEventsByStart();
    m_status = CalendarFetchStatus::OK;
    return true;
}

int CalendarDataModel::getEventCount() const {
    return m_eventCount;
}

const CalendarEvent &CalendarDataModel::getEvent(int index) const {
    return m_events[index];
}

CalendarFetchStatus CalendarDataModel::getStatus() const {
    return m_status;
}

void CalendarDataModel::sortEventsByStart() {
    // Simple insertion sort - bounded by CALENDAR_MAX_EVENTS (~40), trivial cost.
    for (int i = 1; i < m_eventCount; i++) {
        CalendarEvent key = m_events[i];
        int j = i - 1;
        while (j >= 0 && m_events[j].start > key.start) {
            m_events[j + 1] = m_events[j];
            j--;
        }
        m_events[j + 1] = key;
    }
}
