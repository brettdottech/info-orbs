#ifndef PLANE_RADAR_WIDGET_H
#define PLANE_RADAR_WIDGET_H

#include "AdsbClient.h"
#include "RouteClient.h"
#include "Widget.h"

#include <Preferences.h>

class PlaneRadarWidget : public Widget {
public:
    PlaneRadarWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;
    unsigned long getWidgetCyclePageDelayMs() const override;

private:
    struct RangePreset {
        float ring3Km;
        float outerKm;
    };

    struct SortedEntry {
        size_t index;
        float distKm;
    };

    struct DetailSnapshot {
        char callsign[9];
        char desc[36];
        char route[56];
        char alt[12];
        int speedMph;
        uint8_t dbFlags;
        int8_t movement;
        bool occupied;
    };

    struct DrawnPlane {
        int x;
        int y;
        float headingDeg;
        float trackDeg;
        float gsKnots;
        bool isDot;
    };

    static const RangePreset kRangePresets[];
    static constexpr size_t kRangePresetCount = 4;
    static constexpr int kDetailSlots = NUM_SCREENS - 1;

    void cycleRange();
    void saveRangeIndex();
    const RangePreset &currentRange() const;
    float fetchRadiusKm() const;
    void formatRangeLabel(char *buf, size_t len) const;
    void rebuildSortedList();
    float distanceKm(float lat, float lon) const;
    void refreshRouteData();
    int resolveDetailRank(int slot) const;
    void buildDetailSnapshot(int rank, DetailSnapshot *out) const;

    void drawStaticRadarGrid(int screenIndex, bool fullRedraw);
    void drawLoadingRadar(int screenIndex);
    void drawAircraftLayer();
    void eraseOldPlanes();
    void updateCountLabel(unsigned count);

    void updateAircraftDetail(int screenIndex, int rank, const DetailSnapshot &prev, const DetailSnapshot &next);
    void drawEmptyDetail(int screenIndex);
    void drawChangedDetails(bool force = false);

    float m_configLat;
    float m_configLon;
    int m_fetchIntervalSec;
    int m_cycleDelaySec;
    bool m_useMetric;

    uint8_t m_rangeIndex;
    unsigned long m_fetchPrev = 0;
    bool m_dataChanged = true;
    bool m_hasData = false;

    unsigned m_lastAircraftCount = UINT_MAX;

    SortedEntry m_sorted[AdsbClient::kMaxAircraft];
    size_t m_sortedCount = 0;
    size_t m_routeFetchSlot = 0;
    DetailSnapshot m_lastDetail[kDetailSlots];
    char m_slotCallsign[kDetailSlots][9];
    bool m_detailDrawn[kDetailSlots] = {};

    DrawnPlane m_lastDrawnPlanes[AdsbClient::kMaxAircraft];
    size_t m_lastDrawnCount = 0;
    bool m_fullRedrawNeeded = true;

    Preferences m_prefs;
};

#endif
