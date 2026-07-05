# Changelog

Notable changes made in this fork ([SimonGeiger/info-orbs-widget-expansion](https://github.com/SimonGeiger/info-orbs-widget-expansion)), on top of the upstream [brettdottech/info-orbs](https://github.com/brettdottech/info-orbs) project. Format loosely follows [Keep a Changelog](https://keepachangelog.com/), versioned independently from upstream.

## [1.2.0] - 2026-07-06

### Added

- **Calendar widget** — clock, month grid, and a two-mode agenda (next 3 events / next 3 days) synced from any `.ics` feed (Google Calendar, Outlook/Microsoft 365 tested so far). See the [README](README.md#calendar-widget) for setup and [ADR-001](docs/adr/ADR-001-calendar-widget.md) for the full technical design.
  - `RRULE` recurrence expansion: `DAILY`/`WEEKLY`/`MONTHLY`/`YEARLY`, including ordinal `BYDAY` patterns (e.g. "2nd Wednesday of the month").
  - Deduplication of individually-rescheduled or cancelled recurring occurrences (`RECURRENCE-ID`/`EXDATE`).
  - Timezone-aware conversion for a curated set of common named timezones (`TZID`), independent of the device's own configured timezone.
  - Declined and cancelled events are filtered out automatically.
- Embedded Roboto Bold TTF font (`ROBOTO_BOLD`), used to render event titles in bold in the Calendar widget's agenda views.

### Changed

- Welcome screen now shows `version: 1.2.0`.
