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
- `RRULE` recurrence is implemented for the common case only: `FREQ=DAILY`/`WEEKLY` with `INTERVAL`, `COUNT`/`UNTIL`, and a simple `BYDAY` weekday list. `FREQ=MONTHLY`/`YEARLY` and ordinal `BYDAY` (e.g. "2nd Tuesday") are out of scope. `WEEKLY` is kept despite the narrow window — a recurring event's original `DTSTART` is usually long in the past, so without expanding the rule, current occurrences (e.g. a weekly team meeting) would never appear at all.
- `VTIMEZONE` blocks are not parsed. `Z`suffixed UTC timestamps convert via the existing `GlobalTime`/NTP local-time conversion (already DST-aware for the device's own configured zone). Any other timestamp (`TZID` or floating) is treated as already being in the device's local time — correct as long as the calendar and the device share a timezone, which holds for this stationary desk device.
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
3. ICS parser — streaming, line-unfolding, escaping, cancelled-event filtering, all-day support, `RRULE` expansion (daily/weekly subset).
4. Time handling — reuse `GlobalTime`/NTP; treat non-UTC timestamps as already local (no `VTIMEZONE` parsing).
5. Widget class — `WidgetCalendar`; shared slot renderer; month-grid renderer; ongoing-event ring; page state per mode.
6. Empty/error/stale states.

## Open items / future work

- Async HTTP / async parsing for the calendar fetch — not addressed proactively. The combined hourly fetch + RRULE expansion could cause a visible stutter; assess during hardware testing and only pursue this if it's a real problem.
- Display cap (2 events/day + "+N more") and storage cap (`CALENDAR_MAX_EVENTS`, currently 40) are both starting guesses — validate against real calendar data and tune together.
- Extend `RRULE` support to `MONTHLY`/`YEARLY` and ordinal `BYDAY` patterns if a real calendar needs them (not expected to be common for personal use).
- `EXDATE`/`RDATE` (excluding/adding individual recurrence occurrences) are not handled — a recurring event with a one-off cancellation or extra date will not reflect that exception.
- Full `VTIMEZONE` parsing, if treating non-UTC timestamps as local causes visible issues in practice (e.g. events authored in a different timezone than the device).
- Test strategy — collect real `.ics` fixtures from Google, Outlook, and iCloud; unit-test the parser (folding, escaping, all-day, cancelled, `RRULE`) independent of hardware.
- Idea: make the widget configurable via a small on-device web server instead of `config.h`.
- Reference implementations: nconner-06's async HTTP work, dreed47's `WidgetRegistry` PR.
- Idea: add multiple ICS URLs (e.g. work + personal) and merge events into a single agenda, with a per-event source indicator.
