#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include "CalendarDataModel.h"
#include "Widget.h"
#include "config_helper.h"

// Phase 1 (foundation): fetches/parses real ICS data on a two-tier
// schedule (hourly network sync, 10-min local re-evaluation) and proves
// the data path out via a plain-text placeholder draw() on screen 0. Phase
// 2 replaces draw()'s body with the full 5-orb layout (month grid, agenda
// modes/pages, marquee, ongoing-event ring) - CalendarDataModel/IcsParser
// underneath are not expected to change.
class CalendarWidget : public Widget {
public:
    CalendarWidget(ScreenManager &manager, String icsUrl);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    void networkRefreshIfDue(bool force);
    void localReevaluateIfDue();

    String m_icsUrl;
    CalendarDataModel m_dataModel;

    int m_lastFetchedHour = -1; // -1 = never successfully attempted yet
    unsigned long m_lastLocalEvalMillis = 0;
    bool m_needsRedraw = true;
};

#endif // CALENDAR_WIDGET_H
