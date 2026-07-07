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
// EXDATE/ATTENDEE (PARTSTAT=DECLINED only). RRULE expansion covers
// FREQ=DAILY/WEEKLY/MONTHLY/YEARLY with
// INTERVAL, COUNT/UNTIL, a simple (non-ordinal-per-token) BYDAY weekday
// list, single-value BYMONTHDAY, and single-value BYMONTH; anything else
// (RDATE, multi-ordinal BYDAY, BYSETPOS, ...) causes the whole event to be
// dropped rather than partially expanded. No VTIMEZONE-block parsing -
// instead, a small curated table maps common named Windows TZIDs (as seen
// in Outlook exports) to POSIX TZ strings, converted via the standard C
// library's setenv()/tzset()/mktime() (correct DST handling for whatever
// rule the matched zone uses, not a hand-rolled one). An unrecognized TZID,
// or no TZID at all, falls back to treating the timestamp as already being
// in the device's local time.
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

    enum class RRuleFreq { Daily, Weekly, Monthly, Yearly };

    // Parsed RRULE fields, computed once at buffering time rather than
    // re-tokenized at expansion time - see parseRRule()/bufferPendingMaster().
    //
    // Scope note: a single byDayOrdinal applies to all set byDayFlags bits,
    // rather than RFC5545's full per-token-ordinal generality (e.g.
    // "BYDAY=1MO,-1FR" meaning different ordinals per weekday). Every real
    // MONTHLY/YEARLY RRULE seen in practice has exactly one BYDAY token; a
    // hypothetical multi-ordinal rule still parses (last token's ordinal
    // wins) rather than being rejected - graceful degradation, consistent
    // with this parser's "common case only" philosophy.
    struct ParsedRRule {
        RRuleFreq freq = RRuleFreq::Daily;
        int interval = 1;
        int count = -1; // -1 = unbounded by COUNT
        time_t until = 0; // 0 = none specified
        bool hasByDay = false;
        bool byDayFlags[7] = {false, false, false, false, false, false, false}; // 0=Sunday..6=Saturday
        int byDayOrdinal = 0; // 0 = not ordinal; 1..5 = 1st..5th; -1 = last (MONTHLY/YEARLY only)
        bool hasByMonthDay = false;
        int byMonthDay = 0; // 1..31 (single value only - matches all real data)
        bool hasByMonth = false;
        int byMonth = 0; // 1..12 (single value only - matches all real data)
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

        // True if any ATTENDEE line in this VEVENT carries PARTSTAT=DECLINED.
        // A personal/subscription ICS feed typically lists only the
        // subscriber's own attendance status (providers commonly strip other
        // attendees' details for privacy), so in practice this is a
        // locale/provider-independent signal for "I declined this" - see
        // finalizeEvent()'s decline filter. Note: for a feed that does expose
        // a full attendee list (e.g. one exported by the meeting's own
        // organizer), this would also match another attendee's decline.
        bool anyAttendeeDeclined = false;
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

    // Tokenizes an RRULE value's FREQ/INTERVAL/COUNT/UNTIL/BYDAY/BYMONTHDAY/
    // BYMONTH parts into `out`. Returns false if FREQ isn't one of the four
    // supported shapes - the caller then drops the whole event, matching
    // the ADR's "common case only" framing. `localOffsetSeconds` is needed
    // to parse `UNTIL` into the same device-local-epoch space as every other
    // timestamp in this parser (previously hardcoded to 0 - a latent bug,
    // since UNTIL is RFC5545-required to be Z-suffixed but still needs the
    // device offset applied like any other Z-suffixed value).
    bool parseRRule(const String &rrule, int localOffsetSeconds, ParsedRRule &out);

    // Splits an RRULE BYDAY token like "2WE", "-1FR", or a bare "MO" into an
    // optional leading ordinal (outOrdinal, 0 if none present) and the
    // trailing 2-letter weekday code. Returns weekdayCodeToIndex()'s result
    // for the code (-1 if unrecognized).
    int parseByDayToken(const String &token, int &outOrdinal);

    // Weekday index (0=Sunday..6=Saturday, matching weekdayCodeToIndex()) of
    // the calendar date literally encoded in the first 8 characters of a raw
    // ICS date/date-time value (e.g. "20260630T180000") - i.e. the date as
    // authored, with no TZID/UTC-offset adjustment applied. Used to detect
    // when TZID conversion (or a UTC offset) has shifted DTSTART onto a
    // different device-local calendar day than it was written on, so
    // RRULE BYDAY flags (authored relative to that original date) can be
    // rotated to match. Returns 0 (Sunday) if `value` is too short to
    // contain a date - callers only reach that case for malformed input
    // already rejected elsewhere.
    int naiveWeekdayFromDateValue(const String &value);

    // Buffers a recurring VEVENT for expansion after the whole feed has been
    // read (see class-level comment). Silently skips (without consuming a
    // pending-masters slot) a series that has already ended before
    // `windowStart` or hasn't started by `windowEnd` - a real calendar can
    // easily have 100+ recurring series across a year, most of them expired
    // or not-yet-started relative to any given 6-day window, and buffering
    // them anyway would starve the (bounded) table of slots for the
    // handful of series actually active right now. Drops the master
    // (Serial-logged) if its RRULE shape is unsupported or the table is
    // still full after that filter.
    void bufferPendingMaster(const PendingEvent &pending, uint32_t uidHash, time_t baseStart, time_t baseEnd,
                              bool allDay, time_t windowStart, time_t windowEnd, int localOffsetSeconds);

    // Calendar-math helpers for MONTHLY/YEARLY expansion, mirroring
    // CalendarWidget's daysInMonth()/firstWeekdayOfMonth() tmElements_t+
    // makeTime() idiom (duplicated rather than shared - the two classes
    // have no common base, and factoring out ~10 lines for 2 call sites
    // isn't worth it).
    time_t firstOfMonthEpoch(int year, int month); // epoch of day 1, 00:00:00
    int daysInMonthCalc(int year, int month); // via "day 1 of next month minus 1 day"

    // Day-of-month (1-31) of the Nth (ordinal 1..5) or Nth-from-last
    // (ordinal -1..-5, e.g. -1 = last, -2 = 2nd-to-last) occurrence of
    // targetWeekday (0=Sunday..6=Saturday) in month/year. Returns 0 if that
    // ordinal doesn't exist in this month (e.g. "5th Monday" when the month
    // only has 4).
    int nthWeekdayOfMonth(int year, int month, int targetWeekday, int ordinal);

    // Resolves one MONTHLY/YEARLY cycle's candidate occurrence for the given
    // year/month: uses nthWeekdayOfMonth() if rrule.hasByDay, byMonthDay if
    // rrule.hasByMonthDay, or falls back to baseStart's own day-of-month
    // (plain FREQ=MONTHLY/YEARLY with no BY* qualifier). Overlays
    // baseStart's time-of-day. Returns 0 if the day doesn't exist in that
    // month.
    time_t computeMonthlyYearlyCandidate(const ParsedRRule &rrule, time_t baseStart, int year, int month);

    // Looks up `tzid` (a Windows timezone identifier from a TZID= param)
    // against a small curated table and, if found, converts the given naive
    // local calendar fields (as authored in that zone) to true UTC via the
    // C library's own tzset()/mktime() - correctly resolving DST for
    // whatever rule the matched POSIX TZ string encodes. Returns false
    // (leaves outUtc untouched) if tzid isn't in the table.
    bool convertTzidToUtc(const String &tzid, int year, int month, int day, int hour, int minute, int second,
                          time_t &outUtc);

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

    // Caches which POSIX TZ string is currently active (via setenv/tzset),
    // so convertTzidToUtc() can skip the setenv()/tzset() round-trip when
    // consecutive calls share the same zone - common, since most events in
    // one feed use a small handful of distinct TZIDs, and tzset() re-parses
    // its rule string on every call.
    static String m_activePosixTz;
};

#endif // ICS_PARSER_H
