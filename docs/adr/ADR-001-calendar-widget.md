# ADR-001: Calendar Dashboard Widget

> **Naming note:** this ADR refers to the widget class as `WidgetCalendar`. The implementation uses `CalendarWidget` (folder `calendarwidget/`) instead, to match this repo's existing `<Feature>Widget` naming convention (`ClockWidget`, `WeatherWidget`, `MQTTWidget`, `StockWidget`).

- **Status:** Accepted
- **Date:** 2026-07-05
- **Project:** brettdottech/info-orbs (ESP32 + 5x TFT desk widget)

## Context

Info Orbs is an ESP32-based desk display with five round TFT screens, cycling through widgets (clock, weather, stocks, MQTT, etc.) via left/right buttons. This ADR documents the design for a new calendar widget.

## Decisions

**1. Data source: ICS over HTTP**

- Fetch a published `.ics` URL via HTTP(S); avoids OAuth (ESP32 can't handle token storage/refresh).
- Reuse the existing HTTP client pattern (weather/stock widgets already do HTTPS on this hardware) and `StreamUtils` for streaming, line-by-line parsing — no `ArduinoJson`.
- Parser unfolds RFC 5545 continuation lines and unescapes `TEXT` values before reading fields.
- Only `VEVENT`s with a `DTSTART` within the next 6 days are retained (needed for the "day+3..+5" page — see decision 4); earlier/later ones are discarded during parsing to bound memory regardless of feed size.
- `STATUS:CANCELLED` events are dropped during parsing.
- All-day events (`DTSTART;VALUE=DATE`) are supported, displayed without a time.
- `RRULE` recurrence covers `FREQ=DAILY`/`WEEKLY`/`MONTHLY`/`YEARLY` with `INTERVAL`, `COUNT`/`UNTIL`, a `BYDAY` weekday list (including a single ordinal per rule, e.g. "2nd Wednesday" or "last Friday"), and single-value `BYMONTHDAY`/`BYMONTH`. Scope boundaries, chosen because every real RRULE seen in practice fits them: one ordinal applies to all `BYDAY` weekdays in a rule (RFC5545's per-token ordinals, e.g. `BYDAY=1MO,-1FR`, aren't distinguished — the last ordinal-bearing token wins); `BYMONTH`/`BYMONTHDAY` take only the first value of a comma-separated list; `RDATE` and `BYSETPOS` aren't supported. Any RRULE outside this scope causes the whole event to be dropped, matching the original "common case only" decision. `WEEKLY` is kept despite the narrow window — a recurring event's original `DTSTART` is usually long in the past, so without expanding the rule, current occurrences (e.g. a weekly team meeting) would never appear at all. Individually-rescheduled occurrences (a separate `VEVENT` with `RECURRENCE-ID`) and `EXDATE` are both supported — see the parser's class-level comment for the deferred-expansion design this requires.
- `VTIMEZONE` blocks are not parsed. `Z`-suffixed UTC timestamps convert via the existing `GlobalTime`/NTP local-time conversion (already DST-aware for the device's own configured zone). For any other timestamp, a small curated table maps common named Windows `TZID`s (as seen in Outlook exports — e.g. "Pacific Standard Time", "Eastern Standard Time", "India Standard Time") to POSIX TZ rule strings, converted via the standard C library's `setenv()`/`tzset()`/`mktime()` — correct DST handling for whatever rule the matched zone uses, without hand-rolling DST math or taking on a third-party library dependency. An unrecognized `TZID`, or no `TZID` at all (a floating timestamp), still falls back to treating it as already being in the device's local time. When a `TZID`-converted (or UTC-offset-converted) `DTSTART` lands on a different device-local calendar day than it was authored on (e.g. a Pacific-evening meeting becomes early-next-morning in Tokyo), the RRULE's `BYDAY` weekday flags — authored relative to the original date — are rotated by the same day-shift, so expansion checks the correct device-local weekday(s) instead of the pre-shift ones.
- **Evaluated and rejected a ready-made library**: `uICAL` (sourcesimian/uICAL) looked promising (actively maintained, PlatformIO/Arduino-registered, real RRULE test suite) but its `VEvent` class has no `RECURRENCE-ID`/`EXDATE` support at all — adopting it would mean learning a new API while still building a dedup layer around it, for less benefit than extending the already-working, already-verified-against-a-real-calendar parser in this repo. Custom RRULE expansion (RFC5545 subset above) and the standard C library's `setenv`/`tzset`/`mktime` (not a third-party timezone library) were kept instead.
- HTTP client follows redirects (small fixed hop limit).
- Fetch timing — see decision 8 (Data freshness).

**2. Single composite widget in the normal widget cycle**

`WidgetCalendar` plugs into the existing left/right cycling model like clock, weather, and stocks — not a persistent overriding dashboard.

**3. Five-orb layout**

| Orb | Content |
| --- | --- |
| 1 | Clock — reuse existing clock rendering |
| 2 | Month grid — current month, today highlighted, last-synced indicator |
| 3–5 | Agenda area — toggleable between two modes, each with two pages (decision 4) |

**4. Two agenda modes on orbs 3–5, each with two pages**

- **Next 3 events** — page 1: events 1–3; page 2: events 4–6. Title (top line) + time/countdown (bottom line) per orb.
- **Next 3 days** — page 1: today / tomorrow / day+2; page 2: day+3 / day+4 / day+5. Up to 2 events per day, with a "+N more" indicator for busier days.
- A short press (decision 6) toggles between page 1 and page 2; a further short press returns to page 1.
- Switching modes (medium press) resets to page 1.
- If the next event becomes ongoing, its slot shows an orange ring; once it ends, the agenda re-evaluates and the following event takes its place.
- Empty states are context-specific: a day with nothing scheduled (Days mode) shows "No events"; running out of upcoming events altogether (e.g. Events mode page 2, or reaching the end of the window) shows an explicit "No more events in the next 6 days" message instead of a blank slot.

**5. Visual style: text-first**

Event name + time, consistent with the stock ticker widget. Both agenda modes share one "slot renderer" (title line + subtitle line) fed different data per mode.

**6. Button behavior**

- Left / right: cycle widgets (unchanged, system-wide).
- Middle short press: toggle between page 1 and page 2 of the current agenda mode.
- Middle medium press: toggle orbs 3–5 between Next 3 events and Next 3 days.
- Middle long press: trigger a network sync (re-fetch and re-parse the ICS feed).

**7. Connection: single ICS URL in config.h**

- User pastes one provider-generated secret ICS URL into `CALENDAR_ICS_URL` in `config.h`.
- `config.h.template` ships with a placeholder only; the real URL stays in the gitignored `config.h` and is never logged.
- It's a secret link, not a login — anyone with it can read the calendar.

**8. Data freshness**

- Initial fetch happens at boot, alongside NTP sync.
- After that, the calendar re-fetches from the network at the top of every hour, or immediately on a manual long press.
- Independent of network fetches, the agenda is re-evaluated locally every 10 min and on any event start/end boundary, so a finished event drops off without waiting for the next network sync.
- On fetch failure, keep showing the last good data; orb 2's last-synced indicator makes staleness visible.

**9. Event storage**

- Parsed events are kept in a single fixed-size static array (`CalendarEvent events[CALENDAR_MAX_EVENTS]`), repopulated in full on every successful fetch — avoids heap churn/fragmentation from repeated allocation.
- Default capacity: `CALENDAR_MAX_EVENTS = 40`, covering a fairly busy 6-day window with headroom; configurable in `config.h`.
- Each entry stores: start epoch (`uint32_t`), end epoch (`uint32_t`), all-day flag (`bool`), and a fixed-length title buffer (~48 chars, truncated with "…" if longer) — roughly 60–70 bytes/event, ~2.5–3 KB total.
- Kept sorted by start time ascending so both agenda modes (and both pages) read directly from it without re-sorting.
- If the 6-day window holds more events than `CALENDAR_MAX_EVENTS`, the overflow is dropped (array keeps the earliest ones) — a known trade-off, to be validated once real usage data is available.

## Visual design refinements

**Month-grid orb**

- Full 7×6 grid, inscribed in the largest square that fits the circular screen.
- Today highlighted; no per-day event markers.
- Last-synced indicator shown on this orb.
- Weekday header: single-letter initials (M T W T F S S).

**Long event titles**

- Marquee scroll only when a title overflows; slow loop with a brief pause before scrolling starts.

**Color**

- Single accent: reuse `CLOCK_COLOR` across grid highlight and agenda text.
- Ongoing-event ring uses a distinct orange, separate from the accent color.

## Components to build

1. Config layer — `CALENDAR_ICS_URL` (placeholder in template only), 6-day fetch window, `CALENDAR_MAX_EVENTS`.
2. HTTP fetch — reuse existing client; follow redirects.
3. ICS parser — streaming, line-unfolding, escaping, cancelled-event filtering, all-day support, `RRULE` expansion (daily/weekly/monthly/yearly subset), `RECURRENCE-ID`/`EXDATE` dedup.
4. Time handling — reuse `GlobalTime`/NTP for UTC timestamps; a curated Windows-`TZID`-to-POSIX-TZ table plus `setenv`/`tzset`/`mktime` for named-timezone timestamps; treat anything else as already local (no `VTIMEZONE`-block parsing).
5. Widget class — `WidgetCalendar`; shared slot renderer; month-grid renderer; ongoing-event ring; page state per mode.
6. Empty/error/stale states.

## Open items / future work

- Async HTTP / async parsing for the calendar fetch — not addressed proactively. The combined hourly fetch + RRULE expansion could cause a visible stutter; assess during hardware testing and only pursue this if it's a real problem.
- Display cap (2 events/day + "+N more") and storage cap (`CALENDAR_MAX_EVENTS`, currently 40) are both starting guesses — validate against real calendar data and tune together.
- `RDATE` (ad-hoc extra occurrence dates) and `BYSETPOS`, and RFC5545's full per-token `BYDAY` ordinal generality (different ordinals per weekday in one rule, e.g. `BYDAY=1MO,-1FR`) remain unsupported — not seen in real calendars used against this widget so far.
- Full `VTIMEZONE`-block parsing for named zones outside the curated `TZID` table (see Decision 1) — the table currently covers the zones actually seen in testing (Tokyo, Pacific, Eastern, Central, Mountain, India, China, Singapore, UTC, UK, Central Europe); an unlisted zone still falls back to "treat as local".
- Test strategy — collect real `.ics` fixtures from Google, Outlook, and iCloud; unit-test the parser (folding, escaping, all-day, cancelled, `RRULE`, `TZID` conversion) independent of hardware.
- Idea: make the widget configurable via a small on-device web server instead of `config.h`.
- Reference implementations: nconner-06's async HTTP work, dreed47's `WidgetRegistry` PR.
- Idea: add multiple ICS URLs (e.g. work + personal) and merge events into a single agenda, with a per-event source indicator.
