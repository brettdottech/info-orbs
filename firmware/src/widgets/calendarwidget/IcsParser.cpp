#include "IcsParser.h"

#include "config_helper.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace {
// Maps an RRULE BYDAY weekday code to an index matching TimeLib's
// weekday(time_t) - 1 (i.e. 0=Sunday .. 6=Saturday). Ordinal prefixes
// (e.g. "2MO") are not supported and will fail to match, which is fine -
// the caller treats an unrecognized BYDAY entry as simply not set.
int weekdayCodeToIndex(const String &code) {
    if (code == "SU")
        return 0;
    if (code == "MO")
        return 1;
    if (code == "TU")
        return 2;
    if (code == "WE")
        return 3;
    if (code == "TH")
        return 4;
    if (code == "FR")
        return 5;
    if (code == "SA")
        return 6;
    return -1;
}

// Cheap 32-bit fingerprint of a UID string, used so buffered masters/
// suppression entries don't need to store full UIDs (real-world UIDs are
// 100+ char opaque tokens). Collision risk is negligible at the scale this
// parser operates at (at most a few dozen distinct UIDs in flight within a
// 6-day window), and a false-positive collision only ever causes one
// occurrence to be wrongly suppressed, not a crash or corruption.
uint32_t fnv1aHash(const String &s) {
    uint32_t hash = 2166136261u; // FNV offset basis
    for (unsigned int i = 0; i < s.length(); i++) {
        hash ^= (uint8_t)s.charAt(i);
        hash *= 16777619u; // FNV prime
    }
    return hash;
}
} // namespace

// Static (BSS, not stack) - see the rationale in IcsParser.h.
IcsParser::PendingMaster IcsParser::m_pendingMasters[CALENDAR_MAX_PENDING_MASTERS];
int IcsParser::m_pendingMasterCount = 0;
IcsParser::SuppressedOccurrence IcsParser::m_suppressed[CALENDAR_MAX_SUPPRESSED];
int IcsParser::m_suppressedCount = 0;
String IcsParser::m_activePosixTz;

namespace {
// Maps common named Windows/Outlook TZIDs to POSIX TZ rule strings, consumed
// by setenv("TZ", ...)/tzset()/mktime() - these are the standard,
// well-documented values (matching the widely-used posix_tz_db reference
// table), not invented here. POSIX TZ offset sign is inverted from common
// usage (zones west of UTC are positive). mktime() does the actual DST math,
// so EU-rule zones (last Sunday March-last Sunday October) need no separate
// code path from US-rule ones (2nd Sunday March-1st Sunday November) - both
// are just a different rule string.
struct TimezoneEntry {
    const char *tzid;
    const char *posixTz;
};

const TimezoneEntry kTimezoneTable[] = {
    {"Tokyo Standard Time", "JST-9"},
    {"China Standard Time", "CST-8"},
    {"Singapore Standard Time", "SGT-8"},
    {"India Standard Time", "IST-5:30"},
    {"UTC", "UTC0"},
    {"Pacific Standard Time", "PST8PDT,M3.2.0,M11.1.0/2"},
    {"Mountain Standard Time", "MST7MDT,M3.2.0,M11.1.0/2"},
    {"Central Standard Time", "CST6CDT,M3.2.0,M11.1.0/2"},
    {"Eastern Standard Time", "EST5EDT,M3.2.0,M11.1.0/2"},
    {"GMT Standard Time", "GMT0BST,M3.5.0/1,M10.5.0"},
    {"W. Europe Standard Time", "CET-1CEST,M3.5.0,M10.5.0/3"},
};
const int kTimezoneTableSize = sizeof(kTimezoneTable) / sizeof(kTimezoneTable[0]);
} // namespace

IcsParser::IcsParser() {
}

bool IcsParser::readLogicalLine(Stream &stream, String &outLine) {
    String line = stream.readStringUntil('\n');
    if (line.length() == 0) {
        // readStringUntil() returning empty usually means its own read
        // timeout expired without finding '\n' (a genuinely blank input line
        // would also land here, which is fine to also stop on). A single
        // peek()==-1 right after that can be a transient "no bytes buffered
        // this instant" blip on a live, still-open network stream rather
        // than the connection actually being closed - so require it to hold
        // across a few short retries before concluding the feed has ended,
        // to avoid silently truncating the rest of the calendar on a
        // momentary WiFi stall.
        for (int attempt = 0; attempt < 3 && stream.peek() == -1; attempt++) {
            delay(5);
        }
        if (stream.peek() == -1) {
            return false; // end of stream
        }
    }
    if (line.endsWith("\r")) {
        line.remove(line.length() - 1);
    }
    outLine = line;

    // RFC5545 line folding: a continuation line starts with a single space
    // or tab. Keep consuming and appending (with the fold character
    // stripped) until a non-continuation line or EOF is reached.
    while (true) {
        int next = stream.peek();
        if (next != ' ' && next != '\t') {
            break;
        }
        String cont = stream.readStringUntil('\n');
        if (cont.endsWith("\r")) {
            cont.remove(cont.length() - 1);
        }
        if (cont.length() > 0) {
            outLine += cont.substring(1);
        }
    }
    return true;
}

void IcsParser::splitContentLine(const String &line, String &name, String &params, String &value) {
    int colonIdx = line.indexOf(':');
    if (colonIdx == -1) {
        name = line;
        name.trim();
        name.toUpperCase();
        params = "";
        value = "";
        return;
    }

    String head = line.substring(0, colonIdx);
    value = line.substring(colonIdx + 1);

    int semiIdx = head.indexOf(';');
    if (semiIdx == -1) {
        name = head;
        params = "";
    } else {
        name = head.substring(0, semiIdx);
        params = head.substring(semiIdx + 1);
    }
    name.trim();
    name.toUpperCase();
}

bool IcsParser::convertTzidToUtc(const String &tzid, int year, int month, int day, int hour, int minute, int second,
                                  time_t &outUtc) {
    const char *posixTz = nullptr;
    for (int i = 0; i < kTimezoneTableSize; i++) {
        if (tzid == kTimezoneTable[i].tzid) {
            posixTz = kTimezoneTable[i].posixTz;
            break;
        }
    }
    if (posixTz == nullptr) {
        return false; // unrecognized TZID - caller falls back to "treat as local"
    }

    // Skip the setenv()/tzset() round-trip if this zone is already active -
    // tzset() re-parses its rule string every call, and a single feed can
    // have hundreds of TZID-bearing timestamps sharing a handful of zones.
    // Deliberately doesn't save/restore the process's original TZ env var
    // after each call (that would defeat the point of this cache, since the
    // next call would always have to re-set it) - safe because nothing else
    // in this codebase reads the TZ environment variable or calls
    // time()/localtime() (GlobalTime tracks "now" via NTPClient's own
    // manually-applied offset instead), so leaving TZ pointed at whichever
    // zone was last converted has no effect on the rest of the app.
    if (m_activePosixTz != posixTz) {
        setenv("TZ", posixTz, 1);
        tzset();
        m_activePosixTz = posixTz;
    }

    struct tm timeinfo = {};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    timeinfo.tm_isdst = -1; // let mktime() determine DST from the active TZ rule

    time_t utc = mktime(&timeinfo);
    if (utc == (time_t)-1) {
        return false;
    }
    outUtc = utc;
    return true;
}

time_t IcsParser::parseDateTime(const String &value, const String &params, int localOffsetSeconds, bool &outAllDay) {
    String v = value;
    v.trim();
    if (v.length() < 8) {
        outAllDay = false;
        return 0;
    }

    bool isUtc = v.endsWith("Z");
    if (isUtc) {
        v.remove(v.length() - 1);
    }

    int year = v.substring(0, 4).toInt();
    int month = v.substring(4, 6).toInt();
    int day = v.substring(6, 8).toInt();

    bool hasTime = v.length() >= 15 && v.charAt(8) == 'T';
    bool valueIsDate = params.indexOf("VALUE=DATE") != -1 && params.indexOf("VALUE=DATE-TIME") == -1;
    outAllDay = valueIsDate || !hasTime;

    int hour = 0, minute = 0, second = 0;
    if (hasTime) {
        hour = v.substring(9, 11).toInt();
        minute = v.substring(11, 13).toInt();
        second = v.substring(13, 15).toInt();
    }

    if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31) {
        return 0;
    }

    tmElements_t tm;
    tm.Year = year - 1970; // TimeLib's tmElements_t::Year is an offset from 1970
    tm.Month = month;
    tm.Day = day;
    tm.Hour = hour;
    tm.Minute = minute;
    tm.Second = second;
    time_t epoch = makeTime(tm);

    if (isUtc) {
        epoch += localOffsetSeconds; // convert true UTC into the same local-epoch space GlobalTime uses
        return epoch;
    }

    // Not Z-suffixed: check for a recognized TZID before falling back to
    // "treat as already device-local".
    int tzidIdx = params.indexOf("TZID=");
    if (tzidIdx != -1) {
        String tzid = params.substring(tzidIdx + 5);
        int semiIdx = tzid.indexOf(';');
        if (semiIdx != -1) {
            tzid = tzid.substring(0, semiIdx);
        }
        tzid.trim();
        if (tzid.startsWith("\"") && tzid.endsWith("\"") && tzid.length() >= 2) {
            tzid = tzid.substring(1, tzid.length() - 1);
        }

        time_t utc;
        if (convertTzidToUtc(tzid, year, month, day, hour, minute, second, utc)) {
            return utc + localOffsetSeconds; // land in the same device-local-epoch space as everything else
        }
        // unrecognized TZID - fall through to "treat as local" below
    }
    // No TZID, or unrecognized - treated as already local, no adjustment (documented limitation)

    return epoch;
}

String IcsParser::unescapeText(const String &value) {
    String out;
    out.reserve(value.length());
    for (unsigned int i = 0; i < value.length(); i++) {
        char c = value.charAt(i);
        if (c == '\\' && i + 1 < value.length()) {
            char next = value.charAt(i + 1);
            if (next == 'n' || next == 'N') {
                out += '\n';
                i++;
                continue;
            }
            if (next == ',' || next == ';' || next == '\\') {
                out += next;
                i++;
                continue;
            }
        }
        out += c;
    }
    return out;
}

void IcsParser::copyTruncated(const String &text, char outBuf[], int maxLen) {
    if (maxLen <= 0) {
        outBuf[0] = '\0';
        return;
    }
    if ((int)text.length() <= maxLen) {
        strncpy(outBuf, text.c_str(), maxLen);
        outBuf[text.length()] = '\0';
    } else if (maxLen < 3) {
        // Not enough room for the "..." marker (only reachable if a user's
        // config.h sets CALENDAR_TITLE_MAX_LEN/CALENDAR_LOCATION_MAX_LEN
        // below 3) - hard-truncate instead of letting `maxLen - 3` go
        // negative, which would otherwise turn into a huge size_t once it
        // reaches strncpy() and overflow outBuf.
        strncpy(outBuf, text.c_str(), maxLen);
        outBuf[maxLen] = '\0';
    } else {
        String truncated = text.substring(0, maxLen - 3) + "...";
        strncpy(outBuf, truncated.c_str(), maxLen);
        outBuf[maxLen] = '\0';
    }
}

int IcsParser::parseByDayToken(const String &token, int &outOrdinal) {
    outOrdinal = 0;
    int i = 0;
    bool negative = false;
    if (i < (int)token.length() && token.charAt(i) == '-') {
        negative = true;
        i++;
    }
    int numStart = i;
    while (i < (int)token.length() && isDigit(token.charAt(i))) {
        i++;
    }
    if (i > numStart) {
        outOrdinal = token.substring(numStart, i).toInt();
        if (negative) {
            outOrdinal = -outOrdinal;
        }
    }
    String code = token.substring(i);
    return weekdayCodeToIndex(code);
}

bool IcsParser::parseRRule(const String &rrule, int localOffsetSeconds, ParsedRRule &out) {
    String freq;

    int pos = 0;
    while (pos < (int)rrule.length()) {
        int semi = rrule.indexOf(';', pos);
        String part = (semi == -1) ? rrule.substring(pos) : rrule.substring(pos, semi);
        int eq = part.indexOf('=');
        if (eq != -1) {
            String key = part.substring(0, eq);
            String val = part.substring(eq + 1);
            key.toUpperCase();
            if (key == "FREQ") {
                freq = val;
                freq.toUpperCase();
            } else if (key == "INTERVAL") {
                out.interval = val.toInt();
                if (out.interval < 1)
                    out.interval = 1;
            } else if (key == "COUNT") {
                out.count = val.toInt();
            } else if (key == "UNTIL") {
                bool untilAllDayIgnored;
                // UNTIL is RFC5545-required to be Z-suffixed when DTSTART is
                // TZID/local (true for every real example seen) - pass the
                // real localOffsetSeconds (not a hardcoded 0, which was a
                // latent bug: it left `until` in true-UTC space instead of
                // the device-local-epoch space windowStart/windowEnd/every
                // emitted occurrence live in). UNTIL's own params are still
                // "" since it carries no TZID in valid ICS.
                out.until = parseDateTime(val, "", localOffsetSeconds, untilAllDayIgnored);
            } else if (key == "BYDAY") {
                out.hasByDay = true;
                int bpos = 0;
                while (bpos < (int)val.length()) {
                    int comma = val.indexOf(',', bpos);
                    String dayToken = (comma == -1) ? val.substring(bpos) : val.substring(bpos, comma);
                    dayToken.toUpperCase();
                    int ordinal = 0;
                    int idx = parseByDayToken(dayToken, ordinal);
                    if (idx >= 0) {
                        out.byDayFlags[idx] = true;
                        if (ordinal != 0) {
                            out.byDayOrdinal = ordinal; // last ordinal-bearing token wins - see ParsedRRule's scope note
                        }
                    }
                    if (comma == -1)
                        break;
                    bpos = comma + 1;
                }
            } else if (key == "BYMONTHDAY") {
                // Single-value only (matches all real data); a comma-
                // separated list takes just the first value.
                int comma = val.indexOf(',');
                String first = (comma == -1) ? val : val.substring(0, comma);
                out.byMonthDay = first.toInt();
                out.hasByMonthDay = (out.byMonthDay != 0);
            } else if (key == "BYMONTH") {
                int comma = val.indexOf(',');
                String first = (comma == -1) ? val : val.substring(0, comma);
                out.byMonth = first.toInt();
                out.hasByMonth = (out.byMonth >= 1 && out.byMonth <= 12);
            }
        }
        if (semi == -1)
            break;
        pos = semi + 1;
    }

    if (freq == "DAILY") {
        out.freq = RRuleFreq::Daily;
        return true;
    }
    if (freq == "WEEKLY") {
        out.freq = RRuleFreq::Weekly;
        return true;
    }
    if (freq == "MONTHLY") {
        out.freq = RRuleFreq::Monthly;
        return true;
    }
    if (freq == "YEARLY") {
        out.freq = RRuleFreq::Yearly;
        return true;
    }
    return false; // unsupported FREQ - caller drops the whole event
}

int IcsParser::naiveWeekdayFromDateValue(const String &value) {
    if (value.length() < 8) {
        return 0;
    }
    int year = value.substring(0, 4).toInt();
    int month = value.substring(4, 6).toInt();
    int day = value.substring(6, 8).toInt();

    tmElements_t tm;
    tm.Year = year - 1970;
    tm.Month = month;
    tm.Day = day;
    tm.Hour = 0;
    tm.Minute = 0;
    tm.Second = 0;
    return weekday(makeTime(tm)) - 1; // 0=Sunday..6=Saturday
}

void IcsParser::bufferPendingMaster(const PendingEvent &pending, uint32_t uidHash, time_t baseStart, time_t baseEnd,
                                     bool allDay, time_t windowStart, time_t windowEnd, int localOffsetSeconds) {
    ParsedRRule rrule;
    if (!parseRRule(pending.rrule, localOffsetSeconds, rrule)) {
        return; // unsupported RRULE shape - drop the whole event, matching prior behavior
    }

    // BYDAY weekday codes are authored relative to DTSTART's own literal
    // calendar date, but baseStart has already been shifted into
    // device-local-epoch space (by a UTC offset, or via TZID conversion) -
    // which can land on a different calendar day than the digits were
    // written on (e.g. a Pacific evening DTSTART becomes early the next
    // morning in Tokyo). Rotate the flags by the observed day-shift so
    // expansion checks the correct device-local weekday(s) rather than the
    // pre-shift ones - otherwise a series authored as e.g. "every Tuesday"
    // in a zone that shifts a day ahead of the device's zone would silently
    // expand as "every Monday" once converted.
    if (rrule.hasByDay) {
        int naiveWd = naiveWeekdayFromDateValue(pending.dtStartValue);
        int deviceWd = weekday(baseStart) - 1;
        int dayShift = (deviceWd - naiveWd + 7) % 7;
        if (dayShift != 0) {
            bool shifted[7] = {false, false, false, false, false, false, false};
            for (int wd = 0; wd < 7; wd++) {
                if (rrule.byDayFlags[wd]) {
                    shifted[(wd + dayShift) % 7] = true;
                }
            }
            for (int wd = 0; wd < 7; wd++) {
                rrule.byDayFlags[wd] = shifted[wd];
            }
        }
    }

    // A series that already ended before this window, or hasn't started by
    // the time the window closes, can never produce an in-window occurrence
    // - skip it without spending a pending-masters slot. A real calendar can
    // have 100+ recurring series across a year; without this check, expired
    // series from months ago (which appear earlier in a roughly
    // chronological feed) fill the table before this year's still-active
    // series are even reached.
    if ((rrule.until > 0 && rrule.until < windowStart) || baseStart > windowEnd) {
        return;
    }

    if (m_pendingMasterCount >= CALENDAR_MAX_PENDING_MASTERS) {
        Serial.println("IcsParser: pending-masters table full, dropping a recurring series");
        return;
    }

    PendingMaster &m = m_pendingMasters[m_pendingMasterCount++];
    m.uidHash = uidHash;
    m.rrule = rrule;
    m.baseStart = baseStart;
    m.baseEnd = baseEnd;
    m.allDay = allDay;
    copyTruncated(pending.title, m.title, CALENDAR_TITLE_MAX_LEN);
    copyTruncated(pending.location, m.location, CALENDAR_LOCATION_MAX_LEN);
    m.exdateCount = pending.exdateCount;
    for (int i = 0; i < pending.exdateCount; i++) {
        m.exdates[i] = pending.exdates[i];
    }
}

time_t IcsParser::firstOfMonthEpoch(int year, int month) {
    tmElements_t tm;
    tm.Year = year - 1970;
    tm.Month = month;
    tm.Day = 1;
    tm.Hour = 0;
    tm.Minute = 0;
    tm.Second = 0;
    return makeTime(tm);
}

int IcsParser::daysInMonthCalc(int year, int month) {
    int nextMonth = month + 1;
    int nextYear = year;
    if (nextMonth > 12) {
        nextMonth = 1;
        nextYear++;
    }
    time_t firstOfNextMonth = firstOfMonthEpoch(nextYear, nextMonth);
    return day(firstOfNextMonth - 86400);
}

int IcsParser::nthWeekdayOfMonth(int year, int month, int targetWeekday, int ordinal) {
    time_t firstOfMonth = firstOfMonthEpoch(year, month);
    int firstWeekday = weekday(firstOfMonth) - 1; // 0=Sunday..6=Saturday
    int firstOccurrenceDay = 1 + ((targetWeekday - firstWeekday + 7) % 7);
    int totalDays = daysInMonthCalc(year, month);

    if (ordinal < 0) { // counting from the last occurrence (-1 = last, -2 = 2nd-to-last, ...)
        int lastOccurrenceDay = firstOccurrenceDay;
        while (lastOccurrenceDay + 7 <= totalDays) {
            lastOccurrenceDay += 7;
        }
        int candidateDay = lastOccurrenceDay + (ordinal + 1) * 7;
        if (candidateDay < 1) {
            return 0; // that ordinal doesn't exist this month
        }
        return candidateDay;
    }

    int candidateDay = firstOccurrenceDay + (ordinal - 1) * 7;
    if (candidateDay > totalDays) {
        return 0; // that ordinal doesn't exist this month
    }
    return candidateDay;
}

time_t IcsParser::computeMonthlyYearlyCandidate(const ParsedRRule &rrule, time_t baseStart, int year, int month) {
    int dayOfMonth = 0;
    if (rrule.hasByDay) {
        int targetWeekday = -1;
        for (int wd = 0; wd < 7; wd++) {
            if (rrule.byDayFlags[wd]) {
                targetWeekday = wd; // single-BYDAY-token scope - see ParsedRRule's comment
                break;
            }
        }
        if (targetWeekday == -1) {
            return 0;
        }
        int ordinal = (rrule.byDayOrdinal != 0) ? rrule.byDayOrdinal : 1;
        dayOfMonth = nthWeekdayOfMonth(year, month, targetWeekday, ordinal);
    } else if (rrule.hasByMonthDay) {
        dayOfMonth = (rrule.byMonthDay <= daysInMonthCalc(year, month)) ? rrule.byMonthDay : 0;
    } else {
        // Plain FREQ=MONTHLY/YEARLY with no BY* qualifier - repeats on
        // baseStart's own day-of-month.
        int baseDay = day(baseStart);
        dayOfMonth = (baseDay <= daysInMonthCalc(year, month)) ? baseDay : 0;
    }
    if (dayOfMonth <= 0) {
        return 0;
    }

    tmElements_t tm;
    tm.Year = year - 1970;
    tm.Month = month;
    tm.Day = dayOfMonth;
    tm.Hour = hour(baseStart);
    tm.Minute = minute(baseStart);
    tm.Second = second(baseStart);
    return makeTime(tm);
}

void IcsParser::expandRecurrence(const PendingMaster &master, time_t windowStart, time_t windowEnd,
                                  const SuppressedOccurrence suppressed[], int suppressedCount,
                                  CalendarEvent outEvents[], int maxEvents, int &eventsWritten) {
    time_t duration = (master.baseEnd > master.baseStart) ? (master.baseEnd - master.baseStart) : 0;
    time_t effectiveWindowEnd =
        (master.rrule.until > 0 && master.rrule.until < windowEnd) ? master.rrule.until : windowEnd;
    const time_t oneDay = 86400;

    int occurrencesEmitted = 0;

    // An occurrence is skipped if it matches the master's own EXDATE list,
    // or if it's been individually overridden elsewhere in the feed (a
    // separate VEVENT with the same UID and a RECURRENCE-ID equal to this
    // original slot time) - checked against the *original* slot time being
    // generated here, never against wherever an override moved it to.
    auto isSuppressed = [&](time_t occurrence) {
        for (int i = 0; i < master.exdateCount; i++) {
            if (master.exdates[i] == occurrence) {
                return true;
            }
        }
        for (int i = 0; i < suppressedCount; i++) {
            if (suppressed[i].uidHash == master.uidHash && suppressed[i].originalTime == occurrence) {
                return true;
            }
        }
        return false;
    };

    auto emitIfInWindow = [&](time_t occurrence) {
        if (isSuppressed(occurrence)) {
            return;
        }
        // An occurrence is still worth keeping if it hasn't *ended* yet
        // (not merely if it hasn't *started* yet) - otherwise a currently
        // ongoing occurrence (started before windowStart, still running)
        // would be wrongly dropped on the next hourly re-fetch, the same
        // bug fixed for non-recurring events.
        time_t occurrenceEnd = duration > 0 ? occurrence + duration : occurrence;
        if (occurrenceEnd > windowStart && occurrence <= effectiveWindowEnd && eventsWritten < maxEvents) {
            CalendarEvent &e = outEvents[eventsWritten++];
            e.start = occurrence;
            e.end = duration > 0 ? occurrence + duration : 0;
            e.allDay = master.allDay;
            copyTruncated(master.title, e.title, CALENDAR_TITLE_MAX_LEN);
            copyTruncated(master.location, e.location, CALENDAR_LOCATION_MAX_LEN);
        }
    };

    if (master.rrule.freq == RRuleFreq::Daily) {
        time_t step = (time_t)master.rrule.interval * oneDay;
        time_t cursor = master.baseStart;
        // Fast-forward past occurrences that already ended before
        // windowStart, instead of walking one interval at a time from a
        // possibly years-old baseStart (a daily/weekly series with no
        // COUNT/UNTIL would otherwise re-walk its entire history on every
        // hourly re-fetch). occurrencesEmitted is advanced by the same
        // number of steps skipped so COUNT bookkeeping stays correct, and
        // the jump always lands at-or-before the last still-irrelevant
        // occurrence (integer division floors), so no relevant occurrence
        // is skipped.
        if (step > 0 && cursor + duration <= windowStart) {
            time_t elapsed = (windowStart - duration) - cursor;
            long steps = elapsed / step;
            if (steps > 0) {
                cursor += (time_t)steps * step;
                occurrencesEmitted += (int)steps;
            }
        }
        while (cursor <= effectiveWindowEnd) {
            if (master.rrule.count > 0 && occurrencesEmitted >= master.rrule.count)
                break;
            emitIfInWindow(cursor);
            occurrencesEmitted++;
            cursor += step;
        }
    } else if (master.rrule.freq == RRuleFreq::Weekly && !master.rrule.hasByDay) {
        time_t step = (time_t)master.rrule.interval * 7 * oneDay;
        time_t cursor = master.baseStart;
        if (step > 0 && cursor + duration <= windowStart) {
            time_t elapsed = (windowStart - duration) - cursor;
            long steps = elapsed / step;
            if (steps > 0) {
                cursor += (time_t)steps * step;
                occurrencesEmitted += (int)steps;
            }
        }
        while (cursor <= effectiveWindowEnd) {
            if (master.rrule.count > 0 && occurrencesEmitted >= master.rrule.count)
                break;
            emitIfInWindow(cursor);
            occurrencesEmitted++;
            cursor += step;
        }
    } else if (master.rrule.freq == RRuleFreq::Weekly) { // WEEKLY with BYDAY
        int startWeekday = weekday(master.baseStart) - 1; // 0=Sunday..6=Saturday
        time_t weekStart = master.baseStart - (time_t)startWeekday * oneDay;
        while (weekStart <= effectiveWindowEnd) {
            for (int wd = 0; wd < 7; wd++) {
                if (!master.rrule.byDayFlags[wd])
                    continue;
                time_t occurrence = weekStart + (time_t)wd * oneDay;
                if (occurrence < master.baseStart)
                    continue; // series hasn't started yet
                if (occurrence > effectiveWindowEnd)
                    continue;
                // Bounded by the (at most 6-day) window either way, so an
                // approximate COUNT cutoff here (rather than a strict
                // early-exit) is an acceptable simplification.
                if (master.rrule.count > 0 && occurrencesEmitted >= master.rrule.count)
                    continue;
                emitIfInWindow(occurrence);
                occurrencesEmitted++;
            }
            weekStart += (time_t)master.rrule.interval * 7 * oneDay;
        }
    } else { // MONTHLY or YEARLY
        // Loops forward one cycle (month, or year for YEARLY) at a time from
        // the series' own start rather than jump-computing a nearby cycle -
        // see the plan's rationale: for any realistic series age this is at
        // most a few dozen cheap integer-arithmetic iterations, and counting
        // from the true start keeps COUNT semantics correct for free.
        int cycleYear = year(master.baseStart);
        int cycleMonth = master.rrule.hasByMonth ? master.rrule.byMonth : month(master.baseStart);
        // Generous but bounded safety cap guarding against runaway iteration
        // from a malformed UNTIL (~50 years for MONTHLY, ~100 for YEARLY).
        int maxCycles = (master.rrule.freq == RRuleFreq::Monthly) ? 600 : 100;
        for (int cyclesEmitted = 0; cyclesEmitted < maxCycles; cyclesEmitted++) {
            if (master.rrule.count > 0 && occurrencesEmitted >= master.rrule.count)
                break;
            time_t monthStart = firstOfMonthEpoch(cycleYear, cycleMonth);
            if (monthStart > effectiveWindowEnd)
                break;
            time_t candidate = computeMonthlyYearlyCandidate(master.rrule, master.baseStart, cycleYear, cycleMonth);
            if (candidate != 0 && candidate >= master.baseStart) {
                emitIfInWindow(candidate);
                occurrencesEmitted++;
            }
            if (master.rrule.freq == RRuleFreq::Monthly) {
                cycleMonth += master.rrule.interval;
                while (cycleMonth > 12) {
                    cycleMonth -= 12;
                    cycleYear++;
                }
            } else { // YEARLY
                cycleYear += master.rrule.interval;
            }
        }
    }
}

void IcsParser::finalizeAllPendingMasters(time_t windowStart, time_t windowEnd, CalendarEvent outEvents[],
                                           int maxEvents, int &eventsWritten) {
    for (int i = 0; i < m_pendingMasterCount; i++) {
        expandRecurrence(m_pendingMasters[i], windowStart, windowEnd, m_suppressed, m_suppressedCount, outEvents,
                          maxEvents, eventsWritten);
    }
}

void IcsParser::finalizeEvent(const PendingEvent &pending, time_t windowStart, time_t windowEnd,
                               int localOffsetSeconds, CalendarEvent outEvents[], int maxEvents,
                               int &eventsWritten) {
    uint32_t uidHash = pending.uid.length() > 0 ? fnv1aHash(pending.uid) : 0;

    // Record RECURRENCE-ID suppression before any other filter runs - a
    // stale master occurrence must be hidden regardless of whether this
    // override itself ends up displayable (e.g. it was later cancelled or
    // declined; the original slot still shouldn't show the master's version).
    if (pending.hasRecurrenceId && pending.uid.length() > 0) {
        bool recurAllDayIgnored;
        time_t recurTime = parseDateTime(pending.recurrenceIdValue, pending.recurrenceIdParams, localOffsetSeconds,
                                          recurAllDayIgnored);
        if (recurTime > 0 && recurTime >= windowStart && recurTime <= windowEnd) {
            if (m_suppressedCount < CALENDAR_MAX_SUPPRESSED) {
                m_suppressed[m_suppressedCount].uidHash = uidHash;
                m_suppressed[m_suppressedCount].originalTime = recurTime;
                m_suppressedCount++;
            } else {
                Serial.println("IcsParser: suppressed-occurrences table full, a rescheduled/cancelled "
                                "occurrence may show a stale duplicate");
            }
        }
    }

    if (pending.title.length() == 0 || !pending.hasDtStart) {
        return; // incomplete VEVENT, discard
    }

    String status = pending.status;
    status.toUpperCase();
    if (status == "CANCELLED") {
        return;
    }

    // Primary signal: an ATTENDEE line with PARTSTAT=DECLINED (works
    // regardless of calendar provider or locale). Kept alongside the
    // "Declined: " SUMMARY-prefix check as a fallback for feeds that use
    // that Outlook convention instead of (or in addition to) ATTENDEE.
    if (pending.anyAttendeeDeclined || pending.title.startsWith("Declined: ")) {
        return;
    }

    bool allDay = false;
    time_t start = parseDateTime(pending.dtStartValue, pending.dtStartParams, localOffsetSeconds, allDay);
    if (start == 0) {
        return; // unparseable DTSTART
    }

    time_t end = 0;
    if (pending.dtEndValue.length() > 0) {
        bool endAllDayIgnored;
        end = parseDateTime(pending.dtEndValue, pending.dtEndParams, localOffsetSeconds, endAllDayIgnored);
    }

    if (pending.hasRrule) {
        // Buffered for expansion after the whole feed is read, so any
        // RECURRENCE-ID override seen later (or earlier - order doesn't
        // matter, only that both are seen by the time expansion runs) can
        // suppress the stale occurrence this master would otherwise emit.
        bufferPendingMaster(pending, uidHash, start, end, allDay, windowStart, windowEnd, localOffsetSeconds);
        return;
    }

    // Keep the event if it hasn't *ended* yet (not merely if it hasn't
    // *started* yet) - otherwise a currently ongoing event (DTSTART already
    // passed, DTEND still in the future) gets wrongly dropped on the next
    // hourly re-fetch, since its DTSTART looks "in the past" relative to
    // the new windowStart even though the event itself is still current.
    // Point-in-time events with no DTEND fall back to treating DTSTART as
    // the effective end.
    time_t effectiveEnd = (end > 0) ? end : start;
    if (effectiveEnd <= windowStart || start > windowEnd) {
        return; // already fully in the past, or too far in the future
    }
    if (eventsWritten >= maxEvents) {
        return;
    }

    CalendarEvent &e = outEvents[eventsWritten++];
    e.start = start;
    e.end = end;
    e.allDay = allDay;
    copyTruncated(pending.title, e.title, CALENDAR_TITLE_MAX_LEN);
    copyTruncated(pending.location, e.location, CALENDAR_LOCATION_MAX_LEN);
}

int IcsParser::parse(Stream &stream, time_t windowStart, time_t windowEnd, int localOffsetSeconds,
                      CalendarEvent outEvents[], int maxEvents) {
    m_state = State::OUTSIDE;
    m_pendingMasterCount = 0;
    m_suppressedCount = 0;
    int eventsWritten = 0;
    PendingEvent pending;

    String line;
    while (readLogicalLine(stream, line)) {
        String name, params, value;
        splitContentLine(line, name, params, value);

        if (m_state == State::IN_VTIMEZONE) {
            if (name == "END" && value == "VTIMEZONE") {
                m_state = State::OUTSIDE;
            }
            continue;
        }

        if (m_state == State::OUTSIDE) {
            if (name == "BEGIN" && value == "VTIMEZONE") {
                m_state = State::IN_VTIMEZONE;
            } else if (name == "BEGIN" && value == "VEVENT") {
                m_state = State::IN_VEVENT;
                pending = PendingEvent();
            }
            continue;
        }

        // m_state == IN_VEVENT
        if (name == "END" && value == "VEVENT") {
            finalizeEvent(pending, windowStart, windowEnd, localOffsetSeconds, outEvents, maxEvents, eventsWritten);
            m_state = State::OUTSIDE;
            continue;
        }

        if (name == "SUMMARY") {
            pending.title = unescapeText(value);
        } else if (name == "LOCATION") {
            pending.location = unescapeText(value);
        } else if (name == "DTSTART") {
            pending.dtStartValue = value;
            pending.dtStartParams = params;
            pending.hasDtStart = true;
        } else if (name == "DTEND") {
            pending.dtEndValue = value;
            pending.dtEndParams = params;
        } else if (name == "STATUS") {
            pending.status = value;
        } else if (name == "RRULE") {
            pending.rrule = value;
            pending.hasRrule = true;
        } else if (name == "UID") {
            pending.uid = value;
        } else if (name == "RECURRENCE-ID") {
            pending.recurrenceIdValue = value;
            pending.recurrenceIdParams = params;
            pending.hasRecurrenceId = true;
        } else if (name == "ATTENDEE") {
            String upParams = params;
            upParams.toUpperCase();
            if (upParams.indexOf("PARTSTAT=DECLINED") != -1) {
                pending.anyAttendeeDeclined = true;
            }
        } else if (name == "EXDATE") {
            // RFC5545 allows multiple comma-separated dates on one line, and/
            // or multiple EXDATE lines within the same VEVENT (both just
            // accumulate into pending.exdates here). Only in-window values
            // are worth keeping - anything outside [windowStart, windowEnd]
            // could never match an emitted occurrence anyway.
            int epos = 0;
            while (epos < (int)value.length()) {
                int comma = value.indexOf(',', epos);
                String token = (comma == -1) ? value.substring(epos) : value.substring(epos, comma);
                bool exAllDayIgnored;
                time_t exTime = parseDateTime(token, params, localOffsetSeconds, exAllDayIgnored);
                if (exTime > 0 && exTime >= windowStart && exTime <= windowEnd &&
                    pending.exdateCount < CALENDAR_MAX_EXDATES_PER_MASTER) {
                    pending.exdates[pending.exdateCount++] = exTime;
                }
                if (comma == -1)
                    break;
                epos = comma + 1;
            }
        }
        // DESCRIPTION, etc. are still ignored in v1
    }

    // Every RECURRENCE-ID override in the feed has now been seen and
    // recorded, so it's safe to expand the buffered recurring masters.
    finalizeAllPendingMasters(windowStart, windowEnd, outEvents, maxEvents, eventsWritten);

    return eventsWritten;
}
