#ifndef ICS_PARSER_H
#define ICS_PARSER_H

#include "CalendarEvent.h"
#include <Arduino.h>
#include <Stream.h>
#include <TimeLib.h>

// Streaming, line-based ICS (RFC 5545) parser.
//
// Consumes a Stream (e.g. HTTPClient::getStream(), optionally wrapped in a
// ChunkDecodingStream) line by line, without loading the whole response into
// RAM. Only events with a DTSTART falling within [windowStart, windowEnd] are
// kept - the window bounds both which one-off events are retained and how
// far RRULE expansion runs, so memory use stays bounded regardless of feed
// size or how far in the past a recurring event's original DTSTART is.
//
// v1 scope: SUMMARY/DTSTART/DTEND/STATUS/RRULE only. No VTIMEZONE table
// parsing - only Z-suffixed (UTC) timestamps are timezone-correct, everything
// else is treated as already being in the device's local time. RRULE
// expansion covers FREQ=DAILY/WEEKLY with INTERVAL, COUNT/UNTIL, and a simple
// BYDAY weekday list; anything else (MONTHLY/YEARLY, ordinal BYDAY, EXDATE,
// RDATE, ...) causes the whole event to be dropped rather than partially
// expanded.
class IcsParser {
public:
    IcsParser();

    // Parses events from `stream` into `outEvents` (caller-owned array of
    // capacity `maxEvents`). Returns the number of events written.
    //
    // `windowStart`/`windowEnd` and `localOffsetSeconds` are supplied by the
    // caller (rather than read from GlobalTime internally) so this class
    // stays a pure, self-contained unit that can be exercised with fixed
    // inputs for Serial-based testing.
    int parse(Stream &stream, time_t windowStart, time_t windowEnd, int localOffsetSeconds,
              CalendarEvent outEvents[], int maxEvents);

private:
    enum class State {
        OUTSIDE, // not inside VEVENT or VTIMEZONE
        IN_VEVENT,
        IN_VTIMEZONE // skip all content lines until END:VTIMEZONE
    };

    // A VEVENT's properties, gathered while IN_VEVENT, before the
    // END:VEVENT decision (store/expand/discard) is made.
    struct PendingEvent {
        String title;
        String dtStartValue;
        String dtStartParams;
        String dtEndValue;
        String dtEndParams;
        String status;
        String rrule;
        bool hasDtStart = false;
        bool hasRrule = false;
    };

    // Reads one logical (unfolded) content line from stream into `outLine`.
    // Handles RFC5545 line folding: a continuation line starts with a single
    // space or tab and must be appended to the previous logical line. Also
    // strips the trailing \r left by CRLF line endings.
    // Returns false at end of stream (no more lines available).
    bool readLogicalLine(Stream &stream, String &outLine);

    // Parses "PROPERTY;paramA=x;paramB=y:VALUE" into name/params/value.
    void splitContentLine(const String &line, String &name, String &params, String &value);

    // Converts an ICS DATE or DATE-TIME value (e.g. "20260705T140000Z",
    // "20260705T140000", "20260705") into local-epoch seconds (i.e. the same
    // space as GlobalTime::getUnixEpoch()). Sets `outAllDay` true if the
    // value has no "T" time component. Returns 0 (invalid sentinel) if the
    // value doesn't match the expected shape.
    time_t parseDateTime(const String &value, const String &params, int localOffsetSeconds, bool &outAllDay);

    // Un-escapes ICS TEXT value escapes: \n -> newline, \, -> comma,
    // \; -> semicolon, \\ -> backslash.
    String unescapeText(const String &value);

    // Copies `text` into a fixed CALENDAR_TITLE_MAX_LEN buffer, truncating
    // with an ASCII "..." (not a unicode ellipsis, to avoid missing-glyph
    // risk in the project's TTF fonts) if it doesn't fit.
    void copyTitleTruncated(const String &text, char outTitle[]);

    // Expands a supported RRULE (FREQ=DAILY/WEEKLY with INTERVAL, COUNT/
    // UNTIL, and a simple BYDAY weekday list) into individual occurrences
    // within [windowStart, windowEnd]. Returns false (nothing written) if
    // the rule shape isn't supported - the caller then drops the whole
    // event, matching the ADR's "common case only" framing.
    bool expandRecurrence(const PendingEvent &pending, time_t baseStart, time_t baseEnd, bool allDay,
                          time_t windowStart, time_t windowEnd,
                          CalendarEvent outEvents[], int maxEvents, int &eventsWritten);

    // Finishes a PendingEvent at END:VEVENT: applies the STATUS:CANCELLED /
    // window / RRULE decision tree and writes 0-or-more CalendarEvents.
    void finalizeEvent(const PendingEvent &pending, time_t windowStart, time_t windowEnd, int localOffsetSeconds,
                       CalendarEvent outEvents[], int maxEvents, int &eventsWritten);

    State m_state = State::OUTSIDE;
};

#endif // ICS_PARSER_H
