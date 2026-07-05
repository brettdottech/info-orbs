#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include "CalendarDataModel.h"
#include "GlobalTime.h"
#include "Widget.h"
#include "config_helper.h"

// Phase 1 (foundation, done): fetches/parses real ICS data on a two-tier
// schedule (hourly network sync, 10-min local re-evaluation).
// Phase 2a (done): static 5-orb layout - clock, month grid, and an agenda
// always showing "next 3 events".
// Phase 2b (this phase): an alternate "next 3 days" agenda mode, paging
// within either mode (BTN_SHORT/BTN_MEDIUM), an ongoing-event ring, and a
// staleness cue on the sync indicator. CalendarDataModel/IcsParser
// underneath are unchanged.
class CalendarWidget : public Widget {
public:
    CalendarWidget(ScreenManager &manager, String icsUrl);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    enum class AgendaMode { EVENTS, DAYS };

    void networkRefreshIfDue(bool force);
    void localReevaluateIfDue();
    void toggleAgendaPage();
    void changeAgendaMode();

    void drawClock(int screenIndex);
    void drawMonthGrid(int screenIndex);
    void drawAgendaSlot(int screenIndex, const CalendarEvent *event, bool isOngoing,
                         const String &emptyMessage = "--");
    void drawAgendaDaySlot(int screenIndex, time_t dayStart);
    int findNextRelevantEvents(time_t now, const CalendarEvent *outEvents[], int maxCount);
    int collectEventsForDay(time_t dayStart, const CalendarEvent *outEvents[], int maxCount, int &totalCount);
    void splitTextForDisplay(const String &text, int maxCharsPerLine, String &line1, String &line2);
    String formatAgendaDayLabel(time_t eventTime, time_t now); // "Today"/"Tomorrow"/"Mon D"
    String formatAgendaDuration(const CalendarEvent &event); // time range, or "All day"
    String formatEventTime(time_t t); // respects GlobalTime::getFormat24Hour()
    int getClockStamp();
    int daysInMonth(int year, int month);
    int firstWeekdayOfMonth(int year, int month); // 0=Monday..6=Sunday

    String m_icsUrl;
    CalendarDataModel m_dataModel;
    GlobalTime *m_time = nullptr; // set in setup(), mirrors WeatherWidget's m_time

    int m_lastFetchedHour = -1; // -1 = never successfully attempted yet
    unsigned long m_lastLocalEvalMillis = 0;
    bool m_needsRedraw = true;
    int m_lastClockStamp = -1; // hour*60+minute of the last clock-orb redraw; -1 = never drawn
    time_t m_lastSyncEpoch = 0; // local-epoch of the last successful network fetch; 0 = never synced

    AgendaMode m_agendaMode = AgendaMode::EVENTS;
    int m_agendaPage = 0; // 0 or 1
};

#endif // CALENDAR_WIDGET_H
