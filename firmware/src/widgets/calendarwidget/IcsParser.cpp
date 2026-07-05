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
} // namespace

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

bool IcsParser::expandRecurrence(const PendingEvent &pending, time_t baseStart, time_t baseEnd, bool allDay,
                                  time_t windowStart, time_t windowEnd, CalendarEvent outEvents[], int maxEvents,
                                  int &eventsWritten) {
    String freq;
    int interval = 1;
    int count = -1; // -1 = unbounded by COUNT (still bounded by UNTIL/window)
    time_t until = 0; // 0 = none specified
    bool hasByDay = false;
    bool byDayFlags[7] = {false, false, false, false, false, false, false}; // 0=Sunday..6=Saturday

    int pos = 0;
    while (pos < (int)pending.rrule.length()) {
        int semi = pending.rrule.indexOf(';', pos);
        String part = (semi == -1) ? pending.rrule.substring(pos) : pending.rrule.substring(pos, semi);
        int eq = part.indexOf('=');
        if (eq != -1) {
            String key = part.substring(0, eq);
            String val = part.substring(eq + 1);
            key.toUpperCase();
            if (key == "FREQ") {
                freq = val;
                freq.toUpperCase();
            } else if (key == "INTERVAL") {
                interval = val.toInt();
                if (interval < 1)
                    interval = 1;
            } else if (key == "COUNT") {
                count = val.toInt();
            } else if (key == "UNTIL") {
                bool untilAllDayIgnored;
                // UNTIL is normally Z-suffixed UTC per RFC5545; used only as
                // a coarse cutoff alongside windowEnd, so treat with the
                // same offset handling as any other timestamp.
                until = parseDateTime(val, "", 0, untilAllDayIgnored);
            } else if (key == "BYDAY") {
                hasByDay = true;
                int bpos = 0;
                while (bpos < (int)val.length()) {
                    int comma = val.indexOf(',', bpos);
                    String dayCode = (comma == -1) ? val.substring(bpos) : val.substring(bpos, comma);
                    dayCode.toUpperCase();
                    int idx = weekdayCodeToIndex(dayCode);
                    if (idx >= 0) {
                        byDayFlags[idx] = true;
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

    if (freq != "DAILY" && freq != "WEEKLY") {
        return false; // unsupported FREQ - caller drops the whole event
    }

    time_t duration = (baseEnd > baseStart) ? (baseEnd - baseStart) : 0;
    time_t effectiveWindowEnd = (until > 0 && until < windowEnd) ? until : windowEnd;
    const time_t oneDay = 86400;

    int occurrencesEmitted = 0;

    auto emitIfInWindow = [&](time_t occurrence) {
        // An occurrence is still worth keeping if it hasn't *ended* yet
        // (not merely if it hasn't *started* yet) - otherwise a currently
        // ongoing occurrence (started before windowStart, still running)
        // would be wrongly dropped on the next hourly re-fetch, the same
        // bug fixed in finalizeEvent() for non-recurring events.
        time_t occurrenceEnd = duration > 0 ? occurrence + duration : occurrence;
        if (occurrenceEnd > windowStart && occurrence <= effectiveWindowEnd && eventsWritten < maxEvents) {
            CalendarEvent &e = outEvents[eventsWritten++];
            e.start = occurrence;
            e.end = duration > 0 ? occurrence + duration : 0;
            e.allDay = allDay;
            copyTruncated(pending.title, e.title, CALENDAR_TITLE_MAX_LEN);
            copyTruncated(pending.location, e.location, CALENDAR_LOCATION_MAX_LEN);
        }
    };

    if (freq == "DAILY") {
        time_t cursor = baseStart;
        while (cursor <= effectiveWindowEnd) {
            if (count > 0 && occurrencesEmitted >= count)
                break;
            emitIfInWindow(cursor);
            occurrencesEmitted++;
            cursor += (time_t)interval * oneDay;
        }
    } else if (!hasByDay) { // WEEKLY, no BYDAY
        time_t cursor = baseStart;
        while (cursor <= effectiveWindowEnd) {
            if (count > 0 && occurrencesEmitted >= count)
                break;
            emitIfInWindow(cursor);
            occurrencesEmitted++;
            cursor += (time_t)interval * 7 * oneDay;
        }
    } else { // WEEKLY with BYDAY
        int startWeekday = weekday(baseStart) - 1; // 0=Sunday..6=Saturday
        time_t weekStart = baseStart - (time_t)startWeekday * oneDay;
        while (weekStart <= effectiveWindowEnd) {
            for (int wd = 0; wd < 7; wd++) {
                if (!byDayFlags[wd])
                    continue;
                time_t occurrence = weekStart + (time_t)wd * oneDay;
                if (occurrence < baseStart)
                    continue; // series hasn't started yet
                if (occurrence > effectiveWindowEnd)
                    continue;
                // Bounded by the (at most 6-day) window either way, so an
                // approximate COUNT cutoff here (rather than a strict
                // early-exit) is an acceptable simplification.
                if (count > 0 && occurrencesEmitted >= count)
                    continue;
                emitIfInWindow(occurrence);
                occurrencesEmitted++;
            }
            weekStart += (time_t)interval * 7 * oneDay;
        }
    }

    return true;
}

void IcsParser::finalizeEvent(const PendingEvent &pending, time_t windowStart, time_t windowEnd,
                               int localOffsetSeconds, CalendarEvent outEvents[], int maxEvents,
                               int &eventsWritten) {
    if (pending.title.length() == 0 || !pending.hasDtStart) {
        return; // incomplete VEVENT, discard
    }

    String status = pending.status;
    status.toUpperCase();
    if (status == "CANCELLED") {
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
        // Unsupported RRULE shapes drop the whole event rather than falling
        // back to the (likely long-past) base DTSTART.
        expandRecurrence(pending, start, end, allDay, windowStart, windowEnd, outEvents, maxEvents, eventsWritten);
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
        }
        // DESCRIPTION, UID, etc. are still ignored in v1
    }

    return eventsWritten;
}
