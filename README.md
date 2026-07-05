

# Info Orbs — Widget Expansion (Simon Geiger's fork)

This is a personal fork of [brettdottech/info-orbs](https://github.com/brettdottech/info-orbs), an open source ESP32-based desk display with five round TFT screens ("orbs"). This fork adds new widgets on top of the upstream project — currently a **Calendar widget** (see below).

> This fork isn't affiliated with or supported by the upstream project's Discord/community — please only reach out there for questions about the base hardware/firmware, not for issues specific to the widgets added here.

## Getting the hardware & base setup

This fork doesn't change the hardware or the base setup process, so refer to the upstream project for all of that:

- [Get a Dev Kit](https://brett.tech/collections/electronics-projects/products/info-orbs-full-dev-kit) — all the parts you need, pre-picked.
- [Firmware Install / Flashing Guide](references/Firmware%20Install%20Guide.md) and [YouTube assembly/flashing walkthrough](https://link.brett.tech/orbsYT).
- [Upstream README](https://github.com/brettdottech/info-orbs) — wiring diagram, dev environment (PlatformIO) setup, and the full list of existing widgets (clock, weather, stocks, MQTT, parqet.com, custom web data) and their configuration.
- [Discord](https://link.brett.tech/discord) for general project support and community.

Once you have the base project cloned, built, and flashing successfully (per the guides above), come back here for the widgets this fork adds.

## Calendar Widget

Shows your calendar right on the orbs: a clock, a month-at-a-glance grid, and an agenda of upcoming events — synced from any calendar that can publish a standard `.ics` feed (Google Calendar, Outlook/Microsoft 365, iCloud, etc.).

<!-- TODO: add a photo of the calendar widget in action -->

**What it brings you:**
- **Orb 1 — Clock.** Reuses the existing clock widget.
- **Orb 2 — Month grid.** Current month, today highlighted, with an indicator showing how long ago the calendar last synced.
- **Orbs 3–5 — Agenda**, toggleable between two modes:
  - *Next 3 events* — your next few upcoming events, title + time.
  - *Next 3 days* — a day-by-day view of what's coming up, several events per day.
  - An event that's currently happening is highlighted with a ring so it's obvious at a glance.
- Recurring events are supported (daily/weekly/monthly/yearly patterns, including things like "2nd Wednesday of the month"), including individually-rescheduled or cancelled occurrences.
- Declined and cancelled events are automatically filtered out.
- Calendars authored in a different timezone than your device convert correctly for a curated set of common named timezones (see limitations below).

**Setup:**
1. Get a secret `.ics` URL from your calendar provider (in Google Calendar: Settings → your calendar → "Secret address in iCal format"; Outlook: Settings → Shared calendars → "Publish a calendar"). Treat this URL like a password — anyone with it can read your calendar.
2. In `firmware/config/config.h` (see the upstream setup guide for how to create this file from `config.h.template`), set:
   ```c
   #define CALENDAR_ICS_URL "https://your-calendar-provider.example/your-secret-feed.ics"
   ```
3. Optionally raise `CALENDAR_MAX_EVENTS` (default 40) if you have a busy calendar and want more upcoming events kept in memory:
   ```c
   #define CALENDAR_MAX_EVENTS 80
   ```
4. Flash as usual. The calendar syncs automatically at boot and every hour after that.

**Using it:**
- Middle short press — switch between page 1/2 of the current agenda mode.
- Middle medium press — switch orbs 3–5 between "Next 3 events" and "Next 3 days".
- Middle long press — force an immediate re-sync of the calendar.
- Left/right — cycle to other widgets, same as everywhere else.

**Known limitations:** 
- a single `.ics` source at a time; 
- timezones are handled correctly for a curated list of common named zones (not full `VTIMEZONE`-block parsing — see [ADR-001](docs/adr/ADR-001-calendar-widget.md) for the exact scope and rationale, and for anyone wanting the full technical design behind this widget). 
- only been verified against Google Calendar and Outlook/Microsoft 365 `.ics` feeds so far — feedback on other providers (iCloud, etc.) is very welcome.

## License

This project (including this fork's additions) is licensed under AGPLv3 — see [LICENSE.txt](LICENSE.txt).
