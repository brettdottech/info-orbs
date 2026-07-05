#ifndef ICS_PARSER_H
#define ICS_PARSER_H

#include "CalendarEvent.h"
#include "config_helper.h"
#include <Arduino.h>
#include <Stream.h>
#include <TimeLib.h>

// Streaming, line-based ICS (RFC 5545) parser.
//
// Consumes a Stream (e.g. HTTPClient::getStream(), optionally wrapped in a
// ChunkDecodingStream) line by line, without loading the whole response into
// RAM. An event is kept if it hasn't ended yet as of windowStart (so a
// currently-ongoing event whose DTSTART already passed isn't dropped just
// because a later re-fetch's windowStart moved past it) and its DTSTART
// falls no later than windowEnd - the window bounds both which one-off
// events are retained and how far RRULE expansion runs, so memory use stays
// bounded regardless of feed size or how far in the past a recurring
// event's original DTSTART is.
//
// v1 scope: SUMMARY/LOCATION/DTSTART/DTEND/STATUS/RRULE/UID/RECURRENCE-ID/
// EXDATE. No VTIMEZONE table parsing - only Z-suffixed (UTC) timestamps are
// timezone-correct, everything else is treated as already being in the
// device's local time. RRULE expansion covers FREQ=DAILY/WEEKLY with
// INTERVAL, COUNT/UNTIL, and a simple BYDAY weekday list; anything else
// (MONTHLY/YEARLY, ordinal BYDAY, RDATE, ...) causes the whole event to be
// dropped rather than partially expanded.
//
// Individually-rescheduled occurrences of a recurring series (a separate
// VEVENT with RECURRENCE-ID, common in Outlook exports) are deduplicated
// against the master's RRULE expansion: recurring masters are buffered
// during the streaming pass and only expanded once the whole feed has been
// read, so every RECURRENCE-ID override seen anywhere in the feed can
// suppress the corresponding stale occurrence the master would otherwise
// generate. See finalizeAllPendingMasters().
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

    enum class RRuleFreq { Daily, Weekly };

    // Parsed RRULE fields, computed once at buffering time rather than
    // re-tokenized at expansion time - see parseRRule()/bufferPendingMaster().
    struct ParsedRRule {
        RRuleFreq freq = RRuleFreq::Daily;
        int interval = 1;
        int count = -1; // -1 = unbounded by COUNT
        time_t until = 0; // 0 = none specified
        bool hasByDay = false;
        bool byDayFlags[7] = {false, false, false, false, false, false, false}; // 0=Sunday..6=Saturday
    };

    // A recurring VEVENT (has RRULE), buffered instead of expanded
    // immediately, so expansion can happen after the whole feed has been
    // seen (see class-level comment on RECURRENCE-ID deduplication).
    // Deliberately String-free (matches CalendarEvent's fixed-buffer
    // discipline) since these are held for the whole parse() call, not
    // just one VEVENT's lifetime.
    struct PendingMaster {
        uint32_t uidHash = 0; // FNV-1a fingerprint of UID - see fnv1aHash()
        ParsedRRule rrule;
        time_t baseStart = 0;
        time_t baseEnd = 0; // 0 if no DTEND
        bool allDay = false;
        char title[CALENDAR_TITLE_MAX_LEN + 1] = {0};
        char location[CALENDAR_LOCATION_MAX_LEN + 1] = {0};
        time_t exdates[CALENDAR_MAX_EXDATES_PER_MASTER] = {0}; // in-window-only
        int exdateCount = 0;
    };

    // Records that a specific original occurrence of a recurring series
    // (identified by its master's UID + the RECURRENCE-ID timestamp) has
    // been individually overridden elsewhere in the feed, and must be
    // suppressed when the master's RRULE is expanded.
    struct SuppressedOccurrence {
        uint32_t uidHash = 0;
        time_t originalTime = 0; // the RECURRENCE-ID timestamp
    };

    // A VEVENT's properties, gathered while IN_VEVENT, before the
    // END:VEVENT decision (store/buffer/discard) is made.
    struct PendingEvent {
        String title;
        String location;
        String dtStartValue;
        String dtStartParams;
        String dtEndValue;
        String dtEndParams;
        String status;
        String rrule;
        bool hasDtStart = false;
        bool hasRrule = false;

        String uid; // transient - only used to compute a uidHash, not stored long-term
        String recurrenceIdValue;
        String recurrenceIdParams;
        bool hasRecurrenceId = false;
        time_t exdates[CALENDAR_MAX_EXDATES_PER_MASTER] = {0}; // in-window-only
        int exdateCount = 0;
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

    // Copies `text` into a fixed `outBuf` (capacity `maxLen`+1, null-
    // terminated), truncating with an ASCII "..." (not a unicode ellipsis,
    // to avoid missing-glyph risk in the project's TTF fonts) if it doesn't
    // fit. Used for both title (CALENDAR_TITLE_MAX_LEN) and location
    // (CALENDAR_LOCATION_MAX_LEN).
    void copyTruncated(const String &text, char outBuf[], int maxLen);

    // Tokenizes an RRULE value's FREQ/INTERVAL/COUNT/UNTIL/BYDAY parts into
    // `out`. Returns false if FREQ isn't DAILY or WEEKLY (the only shapes
    // this parser expands) - the caller then drops the whole event, matching
    // the ADR's "common case only" framing.
    bool parseRRule(const String &rrule, ParsedRRule &out);

    // Buffers a recurring VEVENT for expansion after the whole feed has been
    // read (see class-level comment). Drops the master (Serial-logged) if
    // its RRULE shape is unsupported or the pending-masters table is full.
    void bufferPendingMaster(const PendingEvent &pending, uint32_t uidHash, time_t baseStart, time_t baseEnd,
                              bool allDay);

    // Expands a buffered master's RRULE into individual occurrences within
    // [windowStart, windowEnd], skipping any occurrence that matches the
    // master's own EXDATE list or an entry in `suppressed` (an
    // individually-overridden occurrence recorded elsewhere in the feed).
    void expandRecurrence(const PendingMaster &master, time_t windowStart, time_t windowEnd,
                          const SuppressedOccurrence suppressed[], int suppressedCount, CalendarEvent outEvents[],
                          int maxEvents, int &eventsWritten);

    // Expands every buffered master (see bufferPendingMaster()) now that the
    // whole feed has been read and every RECURRENCE-ID override is known.
    // Called once by parse() after its read loop ends.
    void finalizeAllPendingMasters(time_t windowStart, time_t windowEnd, CalendarEvent outEvents[], int maxEvents,
                                    int &eventsWritten);

    // Finishes a PendingEvent at END:VEVENT: records RECURRENCE-ID
    // suppression (if any), applies the STATUS:CANCELLED / "Declined: " /
    // window decision tree, and either writes a CalendarEvent directly or
    // buffers a recurring master for later expansion.
    void finalizeEvent(const PendingEvent &pending, time_t windowStart, time_t windowEnd, int localOffsetSeconds,
                       CalendarEvent outEvents[], int maxEvents, int &eventsWritten);

    State m_state = State::OUTSIDE;

    // Buffered recurring masters and known overrides, both reset at the top
    // of each parse() call. `static` (BSS, not stack) deliberately - an
    // IcsParser is constructed as a plain stack-local in
    // CalendarDataModel::fetchAndParse(), and these tables are large enough
    // (~8KB combined) that keeping them as ordinary instance members would
    // put that much on that function's stack frame, risking overflow of the
    // ESP32 Arduino core's default 8KB loop-task stack. Safe as `static`
    // since only one IcsParser is ever active at a time (no re-entrancy).
    static PendingMaster m_pendingMasters[CALENDAR_MAX_PENDING_MASTERS];
    static int m_pendingMasterCount;
    static SuppressedOccurrence m_suppressed[CALENDAR_MAX_SUPPRESSED];
    static int m_suppressedCount;
};

#endif // ICS_PARSER_H
