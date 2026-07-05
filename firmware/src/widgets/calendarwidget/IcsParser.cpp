#include "IcsParser.h"

#include "config_helper.h"
#include <string.h>

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

IcsParser::IcsParser() {
}

bool IcsParser::readLogicalLine(Stream &stream, String &outLine) {
    String line = stream.readStringUntil('\n');
    if (line.length() == 0 && stream.peek() == -1) {
        return false; // end of stream
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
    }
    // else: TZID or floating value - treated as already local, no adjustment (documented Phase 1 limitation)

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
    if ((int)text.length() <= maxLen) {
        strncpy(outBuf, text.c_str(), maxLen);
        outBuf[text.length()] = '\0';
    } else {
        String truncated = text.substring(0, maxLen - 3) + "...";
        strncpy(outBuf, truncated.c_str(), maxLen);
        outBuf[maxLen] = '\0';
    }
}

bool IcsParser::parseRRule(const String &rrule, ParsedRRule &out) {
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
                // UNTIL is normally Z-suffixed UTC per RFC5545; used only as
                // a coarse cutoff alongside windowEnd, so treat with the
                // same offset handling as any other timestamp.
                out.until = parseDateTime(val, "", 0, untilAllDayIgnored);
            } else if (key == "BYDAY") {
                out.hasByDay = true;
                int bpos = 0;
                while (bpos < (int)val.length()) {
                    int comma = val.indexOf(',', bpos);
                    String dayCode = (comma == -1) ? val.substring(bpos) : val.substring(bpos, comma);
                    dayCode.toUpperCase();
                    int idx = weekdayCodeToIndex(dayCode);
                    if (idx >= 0) {
                        out.byDayFlags[idx] = true;
                    }
                    if (comma == -1)
                        break;
                    bpos = comma + 1;
                }
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
    return false; // unsupported FREQ - caller drops the whole event
}

void IcsParser::bufferPendingMaster(const PendingEvent &pending, uint32_t uidHash, time_t baseStart, time_t baseEnd,
                                     bool allDay) {
    ParsedRRule rrule;
    if (!parseRRule(pending.rrule, rrule)) {
        return; // unsupported FREQ (e.g. MONTHLY/YEARLY) - drop the whole event, matching prior behavior
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
        time_t cursor = master.baseStart;
        while (cursor <= effectiveWindowEnd) {
            if (master.rrule.count > 0 && occurrencesEmitted >= master.rrule.count)
                break;
            emitIfInWindow(cursor);
            occurrencesEmitted++;
            cursor += (time_t)master.rrule.interval * oneDay;
        }
    } else if (!master.rrule.hasByDay) { // WEEKLY, no BYDAY
        time_t cursor = master.baseStart;
        while (cursor <= effectiveWindowEnd) {
            if (master.rrule.count > 0 && occurrencesEmitted >= master.rrule.count)
                break;
            emitIfInWindow(cursor);
            occurrencesEmitted++;
            cursor += (time_t)master.rrule.interval * 7 * oneDay;
        }
    } else { // WEEKLY with BYDAY
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
        if (recurTime > 0 && recurTime >= windowStart && recurTime <= windowEnd &&
            m_suppressedCount < CALENDAR_MAX_SUPPRESSED) {
            m_suppressed[m_suppressedCount].uidHash = uidHash;
            m_suppressed[m_suppressedCount].originalTime = recurTime;
            m_suppressedCount++;
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

    // Outlook doesn't expose per-attendee decline via a parseable status
    // field in personal ICS exports - it just prepends "Declined: " to the
    // SUMMARY itself. Filtering on that prefix is the practical equivalent
    // of skipping declined meetings.
    if (pending.title.startsWith("Declined: ")) {
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
        bufferPendingMaster(pending, uidHash, start, end, allDay);
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
