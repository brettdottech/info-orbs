#include "PlaneRadarWidget.h"

#include "config_helper.h"

#include <cmath>
#include <cstring>

namespace {

constexpr uint16_t rgb565Color(uint8_t r, uint8_t g, uint8_t b) {
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

constexpr int kSize = 240;
constexpr int kCenterX = ScreenCenterX;
constexpr int kCenterY = ScreenCenterY;
constexpr int kGridOuterRadius = 107;
constexpr int kRingCount = 4;
constexpr int kCenterDotRadius = 2;
constexpr int kAircraftNoseLenPx = 9;
constexpr int kAircraftTailLenPx = 4;
constexpr int kAircraftTailHalfPx = 5;
constexpr int kAircraftInsideRingInsetPx = kAircraftNoseLenPx + kAircraftTailHalfPx + 1;
constexpr int kBeyondRingDotRadiusPx = 5;
constexpr int kBeyondRingScreenMarginPx = 2;
constexpr float kAircraftTrackHorizonSec = 60.0f;
constexpr int kAircraftSpeedLineMinPx = 2;
constexpr float kAircraftTrackRefOuterKm = 13.3f;
constexpr float kAircraftTrackLengthScale = 1.5f / 5.0f;
constexpr float kKmPerDeg = 111.0f;
constexpr float kKmPerMile = 1.609344f;
constexpr char kPrefsNamespace[] = "planeradar";
constexpr char kPrefsRangeKey[] = "rangeIdx";

const uint16_t kColorBackground = rgb565Color(4, 10, 28);
const uint16_t kColorGrid = rgb565Color(16, 100, 32);
const uint16_t kColorLabel = TFT_WHITE;
const uint16_t kColorAircraft = TFT_YELLOW;
const uint16_t kColorTrack = 0xF81F;
const uint16_t kColorType = rgb565Color(255, 200, 0);
const uint16_t kColorAlt = rgb565Color(90, 200, 255);
const uint16_t kColorMil = TFT_RED;
const uint16_t kColorInteresting = rgb565Color(0, 200, 255);
const uint16_t kColorPia = 0xF81F;
const uint16_t kColorLadd = TFT_DARKGREY;

constexpr float kKtToMph = 1.15078f;
constexpr int kSpeedRedrawThresholdMph = 2;
constexpr uint8_t kDbFlagMilitary = 1;
constexpr uint8_t kDbFlagInteresting = 2;
constexpr uint8_t kDbFlagPia = 4;
constexpr uint8_t kDbFlagLadd = 8;

enum class MovementHint : int8_t {
    Unknown = 0,
    Arriving,
    Departing,
};

const uint16_t kColorArriving = rgb565Color(80, 220, 120);
const uint16_t kColorDeparting = rgb565Color(255, 180, 60);

MovementHint classifyMovement(const AdsbClient::Aircraft &plane) {
    if (plane.gsKnots < 40.0f || !plane.hasBaroRate) {
        return MovementHint::Unknown;
    }
    if (plane.baroRateFpm <= -250) {
        return MovementHint::Arriving;
    }
    if (plane.baroRateFpm >= 250) {
        return MovementHint::Departing;
    }
    return MovementHint::Unknown;
}

const char *movementLabel(MovementHint hint) {
    switch (hint) {
    case MovementHint::Arriving:
        return "Arriving";
    case MovementHint::Departing:
        return "Departing";
    default:
        return nullptr;
    }
}

uint16_t movementColor(MovementHint hint) {
    switch (hint) {
    case MovementHint::Arriving:
        return kColorArriving;
    case MovementHint::Departing:
        return kColorDeparting;
    default:
        return kColorLabel;
    }
}

bool hasSpecialDbFlags(uint8_t flags) {
    return (flags & (kDbFlagMilitary | kDbFlagInteresting | kDbFlagPia | kDbFlagLadd)) != 0;
}

uint16_t dbFlagsOutlineColor(uint8_t flags) {
    if (flags & kDbFlagMilitary) {
        return kColorMil;
    }
    if (flags & kDbFlagPia) {
        return kColorPia;
    }
    if (flags & kDbFlagLadd) {
        return kColorLadd;
    }
    if (flags & kDbFlagInteresting) {
        return kColorInteresting;
    }
    return kColorLabel;
}

void formatDbFlagsLabel(uint8_t flags, char *buf, size_t len) {
    buf[0] = '\0';
    if (len == 0 || !hasSpecialDbFlags(flags)) {
        return;
    }

    bool first = true;
    auto append = [&](const char *label) {
        if (!first) {
            strncat(buf, " · ", len - strlen(buf) - 1);
        }
        strncat(buf, label, len - strlen(buf) - 1);
        first = false;
    };

    if (flags & kDbFlagMilitary) {
        append("MILITARY");
    }
    if (flags & kDbFlagInteresting) {
        append("INTERESTING");
    }
    if (flags & kDbFlagPia) {
        append("PIA");
    }
    if (flags & kDbFlagLadd) {
        append("LADD");
    }
}

void drawSpecialOrbOutline(ScreenManager &sm, uint16_t color) {
    sm.drawCircle(kCenterX, kCenterY, 118, color);
    sm.drawCircle(kCenterX, kCenterY, 117, color);
}

int knotsToMph(float knots) {
    return static_cast<int>(lroundf(knots * kKtToMph));
}

int distSqFromCenter(int x, int y) {
    const int dx = x - kCenterX;
    const int dy = y - kCenterY;
    return dx * dx + dy * dy;
}

void drawThickLine(ScreenManager &sm, int x0, int y0, int x1, int y1, uint16_t color) {
    sm.drawLine(x0, y0, x1, y1, color);
    sm.drawLine(x0 + 1, y0, x1 + 1, y1, color);
}

void drawGridRing(ScreenManager &sm, int cx, int cy, int r, uint16_t color) {
    if (r <= 0) {
        return;
    }
    for (int i = 0; i < 2 && r - i > 0; ++i) {
        sm.drawCircle(cx, cy, r - i, color);
    }
}

void drawCardinal(ScreenManager &sm, const char *text, int x, int y, uint8_t datum) {
    sm.setLegacyTextColor(kColorLabel, kColorBackground);
    sm.setLegacyTextDatum(datum);
    sm.setLegacyTextSize(1);
    sm.drawLegacyString(text, x, y, 2);
}

float innerRingMaxKm(float outerKm) {
    return outerKm * (static_cast<float>(kGridOuterRadius - kAircraftInsideRingInsetPx) /
                      static_cast<float>(kGridOuterRadius));
}

void offsetKmFromCenter(double centerLat, double centerLon, float lat, float lon, float *dxKm, float *dyKm,
                        float *distKm) {
    *dxKm = static_cast<float>(lon - centerLon) * kKmPerDeg;
    *dyKm = static_cast<float>(lat - centerLat) * kKmPerDeg;
    *distKm = sqrtf((*dxKm) * (*dxKm) + (*dyKm) * (*dyKm));
}

void latLonToScreen(double centerLat, double centerLon, float outerKm, float lat, float lon, int *outX,
                    int *outY) {
    const float pxPerKm = static_cast<float>(kGridOuterRadius) / outerKm;
    float dxKm = 0.0f;
    float dyKm = 0.0f;
    float distKm = 0.0f;
    offsetKmFromCenter(centerLat, centerLon, lat, lon, &dxKm, &dyKm, &distKm);
    *outX = kCenterX + static_cast<int>(lroundf(dxKm * pxPerKm));
    *outY = kCenterY - static_cast<int>(lroundf(dyKm * pxPerKm));
}

bool beyondRingEdgeDot(double centerLat, double centerLon, float outerKm, float lat, float lon, int *outX,
                       int *outY) {
    float dxKm = 0.0f;
    float dyKm = 0.0f;
    float distKm = 0.0f;
    offsetKmFromCenter(centerLat, centerLon, lat, lon, &dxKm, &dyKm, &distKm);
    if (distKm < 0.01f || distKm <= innerRingMaxKm(outerKm)) {
        return false;
    }

    const int rimR = kCenterX - kBeyondRingScreenMarginPx;
    const float angleRad = atan2f(dxKm, dyKm);
    *outX = kCenterX + static_cast<int>(lroundf(sinf(angleRad) * rimR));
    *outY = kCenterY - static_cast<int>(lroundf(cosf(angleRad) * rimR));
    return true;
}

void noseTip(int cx, int cy, float headingDeg, int *tipX, int *tipY) {
    const float rad = headingDeg * DEG_TO_RAD;
    *tipX = cx + static_cast<int>(lroundf(sinf(rad) * kAircraftNoseLenPx));
    *tipY = cy - static_cast<int>(lroundf(cosf(rad) * kAircraftNoseLenPx));
}

void clipPointToOuterRing(int x0, int y0, int *x1, int *y1) {
    const int maxRSq = kGridOuterRadius * kGridOuterRadius;
    if (distSqFromCenter(*x1, *y1) <= maxRSq) {
        return;
    }

    const int dx = *x1 - x0;
    const int dy = *y1 - y0;
    float t = 1.0f;
    for (int step = 0; step < 20; ++step) {
        const int px = x0 + static_cast<int>(lroundf(dx * t));
        const int py = y0 + static_cast<int>(lroundf(dy * t));
        if (distSqFromCenter(px, py) <= maxRSq) {
            *x1 = px;
            *y1 = py;
            return;
        }
        t -= 0.05f;
        if (t <= 0.0f) {
            *x1 = x0;
            *y1 = y0;
            return;
        }
    }
}

int speedLineLengthPx(float gsKnots) {
    if (gsKnots <= 0.0f) {
        return 0;
    }
    constexpr float kKmPerKnotPerHorizon = 1.852f * kAircraftTrackHorizonSec / 3600.0f;
    const float px = gsKnots * kKmPerKnotPerHorizon * kGridOuterRadius / kAircraftTrackRefOuterKm *
                     kAircraftTrackLengthScale;
    const int len = static_cast<int>(px + 0.5f);
    return len < kAircraftSpeedLineMinPx ? kAircraftSpeedLineMinPx : len;
}

void drawHeadingTriangle(ScreenManager &sm, int cx, int cy, float headingDeg, uint16_t color) {
    const float rad = headingDeg * DEG_TO_RAD;
    const float sinH = sinf(rad);
    const float cosH = cosf(rad);

    int tipX = 0;
    int tipY = 0;
    noseTip(cx, cy, headingDeg, &tipX, &tipY);

    const int baseX = cx - static_cast<int>(lroundf(sinH * kAircraftTailLenPx));
    const int baseY = cy + static_cast<int>(lroundf(cosH * kAircraftTailLenPx));
    const int wingX = static_cast<int>(lroundf(cosH * kAircraftTailHalfPx));
    const int wingY = static_cast<int>(lroundf(sinH * kAircraftTailHalfPx));

    sm.fillTriangle(tipX, tipY, baseX + wingX, baseY + wingY, baseX - wingX, baseY - wingY, color);
}

void drawSpeedVector(ScreenManager &sm, int cx, int cy, float headingDeg, float trackDeg, float gsKnots,
                     uint16_t color) {
    const int len = speedLineLengthPx(gsKnots);
    if (len <= 0) {
        return;
    }

    int tipX = 0;
    int tipY = 0;
    noseTip(cx, cy, headingDeg, &tipX, &tipY);

    const float rad = trackDeg * DEG_TO_RAD;
    int ex = tipX + static_cast<int>(lroundf(sinf(rad) * len));
    int ey = tipY - static_cast<int>(lroundf(cosf(rad) * len));
    clipPointToOuterRing(tipX, tipY, &ex, &ey);
    if (ex == tipX && ey == tipY) {
        return;
    }
    drawThickLine(sm, tipX, tipY, ex, ey, color);
}

struct DrawItem {
    size_t index = 0;
    int x = 0;
    int y = 0;
    int distSq = 0;
    bool isRim = false;
};

void sortDrawItemsFarFirst(DrawItem *items, size_t count) {
    for (size_t i = 1; i < count; ++i) {
        const DrawItem key = items[i];
        size_t j = i;
        while (j > 0 && items[j - 1].distSq < key.distSq) {
            items[j] = items[j - 1];
            --j;
        }
        items[j] = key;
    }
}

} // namespace

const PlaneRadarWidget::RangePreset PlaneRadarWidget::kRangePresets[] = {
    {5.0f, 5.0f * 4.0f / 3.0f},
    {10.0f, 10.0f * 4.0f / 3.0f},
    {15.0f, 15.0f * 4.0f / 3.0f},
    {25.0f, 25.0f * 4.0f / 3.0f},
};

PlaneRadarWidget::PlaneRadarWidget(ScreenManager &manager, ConfigManager &config)
    : Widget(manager, config),
      m_configLat(PLANE_RADAR_LAT),
      m_configLon(PLANE_RADAR_LON),
      m_fetchIntervalSec(PLANE_RADAR_FETCH_INTERVAL_SEC),
      m_cycleDelaySec(PLANE_RADAR_CYCLE_DELAY),
#ifdef PLANE_RADAR_RANGE_INDEX
      m_rangeIndex(PLANE_RADAR_RANGE_INDEX),
#else
      m_rangeIndex(1),
#endif
#ifdef PLANE_RADAR_UNITS_METRIC
      m_useMetric(true)
#else
      m_useMetric(false)
#endif
{
    m_enabled = (INCLUDE_PLANE_RADAR == WIDGET_ON);
    m_config.addConfigBool("PlaneRadarWidget", "planeEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigFloat("PlaneRadarWidget", "planeLat", &m_configLat, "Radar center latitude");
    m_config.addConfigFloat("PlaneRadarWidget", "planeLon", &m_configLon, "Radar center longitude");
    m_config.addConfigInt("PlaneRadarWidget", "planeFetchSec", &m_fetchIntervalSec, "ADS-B poll interval (seconds)", true);
    m_config.addConfigInt("PlaneRadarWidget", "planeCycleSec", &m_cycleDelaySec, "Seconds on Plane Radar before carousel advances", true);
    m_config.addConfigBool("PlaneRadarWidget", "planeUnitsMetric", &m_useMetric, "Use metric units on range label", true);

    for (int i = 0; i < kDetailSlots; ++i) {
        m_lastDetail[i].occupied = false;
        m_lastDetail[i].callsign[0] = '\0';
        m_slotCallsign[i][0] = '\0';
    }
}

unsigned long PlaneRadarWidget::getWidgetCyclePageDelayMs() const {
    return static_cast<unsigned long>(m_cycleDelaySec) * 1000UL;
}

void PlaneRadarWidget::setup() {
    m_fetchPrev = 0;
    m_dataChanged = true;
    m_hasData = false;
    m_sortedCount = 0;
    m_routeFetchSlot = 0;
    m_lastAircraftCount = UINT_MAX;
    m_fullRedrawNeeded = true;
    m_lastDrawnCount = 0;
    RouteClient::clearCache();
    for (int i = 0; i < kDetailSlots; ++i) {
        m_lastDetail[i].occupied = false;
        m_lastDetail[i].route[0] = '\0';
        m_slotCallsign[i][0] = '\0';
    }

    if (m_prefs.begin(kPrefsNamespace, true)) {
        const uint8_t saved = m_prefs.getUChar(kPrefsRangeKey, m_rangeIndex);
        m_rangeIndex = saved < kRangePresetCount ? saved : m_rangeIndex;
        m_prefs.end();
    }
}

void PlaneRadarWidget::saveRangeIndex() {
    if (m_prefs.begin(kPrefsNamespace, false)) {
        m_prefs.putUChar(kPrefsRangeKey, m_rangeIndex);
        m_prefs.end();
    }
}

void PlaneRadarWidget::cycleRange() {
    m_rangeIndex = static_cast<uint8_t>((m_rangeIndex + 1) % kRangePresetCount);
    saveRangeIndex();
    m_fetchPrev = 0;
    m_dataChanged = true;
    m_fullRedrawNeeded = true;
    for (int i = 0; i < kDetailSlots; ++i) {
        m_slotCallsign[i][0] = '\0';
    }
}

const PlaneRadarWidget::RangePreset &PlaneRadarWidget::currentRange() const {
    return kRangePresets[m_rangeIndex];
}

float PlaneRadarWidget::fetchRadiusKm() const {
    const float outerKm = currentRange().outerKm;
    const float screenRPx = static_cast<float>(kCenterX - kBeyondRingScreenMarginPx);
    return outerKm * (screenRPx / static_cast<float>(kGridOuterRadius));
}

void PlaneRadarWidget::formatRangeLabel(char *buf, size_t len) const {
    const float ring3Km = currentRange().ring3Km;
    if (m_useMetric) {
        snprintf(buf, len, "%dkm", static_cast<int>(lroundf(ring3Km)));
    } else {
        snprintf(buf, len, "%dmi", static_cast<int>(lroundf(ring3Km / kKmPerMile)));
    }
}

float PlaneRadarWidget::distanceKm(float lat, float lon) const {
    float dxKm = 0.0f;
    float dyKm = 0.0f;
    float distKm = 0.0f;
    offsetKmFromCenter(m_configLat, m_configLon, lat, lon, &dxKm, &dyKm, &distKm);
    return distKm;
}

void PlaneRadarWidget::rebuildSortedList() {
    m_sortedCount = 0;
    const size_t n = AdsbClient::aircraftCount();
    const AdsbClient::Aircraft *planes = AdsbClient::aircraftList();

    for (size_t i = 0; i < n && m_sortedCount < AdsbClient::kMaxAircraft; ++i) {
        m_sorted[m_sortedCount].index = i;
        m_sorted[m_sortedCount].distKm = distanceKm(planes[i].lat, planes[i].lon);
        ++m_sortedCount;
    }

    for (size_t i = 1; i < m_sortedCount; ++i) {
        const SortedEntry key = m_sorted[i];
        size_t j = i;
        while (j > 0 && m_sorted[j - 1].distKm > key.distKm) {
            m_sorted[j] = m_sorted[j - 1];
            --j;
        }
        m_sorted[j] = key;
    }
}

void PlaneRadarWidget::update(bool force) {
    if (!force && m_fetchPrev != 0 && (millis() - m_fetchPrev) < static_cast<unsigned long>(m_fetchIntervalSec) * 1000UL) {
        return;
    }

    setBusy(true);
    bool ok = false;
    if (force) {
        int retry = 0;
        while (!(ok = AdsbClient::fetchUpdate(m_configLat, m_configLon, fetchRadiusKm())) && retry++ < MAX_RETRIES) {
        }
    } else {
        ok = AdsbClient::fetchUpdate(m_configLat, m_configLon, fetchRadiusKm());
    }
    setBusy(false);

    m_fetchPrev = millis();
    if (ok) {
        rebuildSortedList();
        m_hasData = true;
        m_dataChanged = true;
        refreshRouteData();
    }
}

void PlaneRadarWidget::refreshRouteData() {
    const char *callsigns[kDetailSlots];
    size_t count = 0;
    for (int i = 0; i < kDetailSlots; ++i) {
        const int rank = resolveDetailRank(i);
        if (rank >= 0) {
            callsigns[count++] = AdsbClient::aircraftList()[m_sorted[rank].index].callsign;
        }
    }
    if (count == 0) {
        return;
    }

    setBusy(true);
    const bool fetched = RouteClient::prefetchOne(callsigns, count, &m_routeFetchSlot);
    setBusy(false);

    if (fetched) {
        m_dataChanged = true;
    }
}

void PlaneRadarWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);
    if (!force && !m_dataChanged) {
        return;
    }

    if (!m_hasData) {
        if (force) {
            drawLoadingRadar(0);
            m_fullRedrawNeeded = true;
        }
        m_dataChanged = false;
        return;
    }

    if (force || m_fullRedrawNeeded) {
        drawStaticRadarGrid(0, true);
        m_fullRedrawNeeded = false;
    } else {
        eraseOldPlanes();
        drawStaticRadarGrid(0, false);
    }

    drawAircraftLayer();
    drawChangedDetails();
    m_dataChanged = false;
}

void PlaneRadarWidget::drawLoadingRadar(int screenIndex) {
    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(kColorBackground);
    m_manager.drawString("Plane Radar", kCenterX, kCenterY - 20, 22, Align::MiddleCenter, kColorLabel,
                         kColorBackground);
    m_manager.drawString("Loading...", kCenterX, kCenterY + 20, 18, Align::MiddleCenter, kColorLabel,
                         kColorBackground);
}

void PlaneRadarWidget::drawStaticRadarGrid(int screenIndex, bool fullRedraw) {
    m_manager.selectScreen(screenIndex);
    if (fullRedraw) {
        m_manager.fillScreen(kColorBackground);
    }

    drawGridRing(m_manager, kCenterX, kCenterY, kGridOuterRadius, kColorGrid);
    for (int i = 1; i < kRingCount; ++i) {
        const int r = (kGridOuterRadius * i) / kRingCount;
        drawGridRing(m_manager, kCenterX, kCenterY, r, kColorGrid);
    }

    drawThickLine(m_manager, kCenterX, kCenterY - kGridOuterRadius, kCenterX, kCenterY + kGridOuterRadius,
                  kColorGrid);
    drawThickLine(m_manager, kCenterX - kGridOuterRadius, kCenterY, kCenterX + kGridOuterRadius, kCenterY,
                  kColorGrid);

    m_manager.fillCircle(kCenterX, kCenterY, kCenterDotRadius, kColorLabel);

    drawCardinal(m_manager, "N", kCenterX, 2, TC_DATUM);
    drawCardinal(m_manager, "S", kCenterX, kSize - 2, BC_DATUM);
    drawCardinal(m_manager, "W", 2, kCenterY, ML_DATUM);
    drawCardinal(m_manager, "E", kSize - 2, kCenterY, MR_DATUM);

    char rangeLabel[12];
    formatRangeLabel(rangeLabel, sizeof(rangeLabel));
    m_manager.setLegacyTextColor(kColorGrid, kColorBackground);
    m_manager.setLegacyTextDatum(MR_DATUM);
    m_manager.drawLegacyString(rangeLabel, kCenterX + kGridOuterRadius - 6, kCenterY, 2);

    if (fullRedraw) {
        m_lastAircraftCount = UINT_MAX;
        updateCountLabel(static_cast<unsigned>(AdsbClient::aircraftCount()));
    }
}

void PlaneRadarWidget::eraseOldPlanes() {
    m_manager.selectScreen(0);
    for (size_t i = 0; i < m_lastDrawnCount; ++i) {
        const DrawnPlane &p = m_lastDrawnPlanes[i];
        if (p.isDot) {
            m_manager.fillCircle(p.x, p.y, kBeyondRingDotRadiusPx, kColorBackground);
        } else {
            drawSpeedVector(m_manager, p.x, p.y, p.headingDeg, p.trackDeg, p.gsKnots, kColorBackground);
            drawHeadingTriangle(m_manager, p.x, p.y, p.headingDeg, kColorBackground);
        }
    }
    m_lastDrawnCount = 0;
}

void PlaneRadarWidget::drawAircraftLayer() {
    m_manager.selectScreen(0);

    const float outerKm = currentRange().outerKm;
    const float maxDistKm = innerRingMaxKm(outerKm);
    const size_t n = AdsbClient::aircraftCount();
    const AdsbClient::Aircraft *planes = AdsbClient::aircraftList();

    DrawItem items[AdsbClient::kMaxAircraft];
    size_t drawCount = 0;
    m_lastDrawnCount = 0;

    for (size_t i = 0; i < n; ++i) {
        float dxKm = 0.0f;
        float dyKm = 0.0f;
        float distKm = 0.0f;
        offsetKmFromCenter(m_configLat, m_configLon, planes[i].lat, planes[i].lon, &dxKm, &dyKm, &distKm);

        if (distKm <= maxDistKm) {
            int x = 0;
            int y = 0;
            latLonToScreen(m_configLat, m_configLon, outerKm, planes[i].lat, planes[i].lon, &x, &y);
            items[drawCount].index = i;
            items[drawCount].x = x;
            items[drawCount].y = y;
            items[drawCount].distSq = distSqFromCenter(x, y);
            items[drawCount].isRim = false;
            ++drawCount;
            
            if (m_lastDrawnCount < AdsbClient::kMaxAircraft) {
                m_lastDrawnPlanes[m_lastDrawnCount++] = {x, y, planes[i].noseDeg, planes[i].trackDeg, planes[i].gsKnots, false};
            }
            continue;
        }

        int dotX = 0;
        int dotY = 0;
        if (beyondRingEdgeDot(m_configLat, m_configLon, outerKm, planes[i].lat, planes[i].lon, &dotX, &dotY)) {
            m_manager.fillCircle(dotX, dotY, kBeyondRingDotRadiusPx, kColorAircraft);
            if (m_lastDrawnCount < AdsbClient::kMaxAircraft) {
                m_lastDrawnPlanes[m_lastDrawnCount++] = {dotX, dotY, 0.0f, 0.0f, 0.0f, true};
            }
        }
    }

    sortDrawItemsFarFirst(items, drawCount);
    for (size_t d = 0; d < drawCount; ++d) {
        const size_t i = items[d].index;
        drawSpeedVector(m_manager, items[d].x, items[d].y, planes[i].noseDeg, planes[i].trackDeg,
                        planes[i].gsKnots, kColorTrack);
    }
    for (size_t d = 0; d < drawCount; ++d) {
        const size_t i = items[d].index;
        drawHeadingTriangle(m_manager, items[d].x, items[d].y, planes[i].noseDeg, kColorAircraft);
    }

    updateCountLabel(static_cast<unsigned>(n));
}

void PlaneRadarWidget::updateCountLabel(unsigned count) {
    if (count == m_lastAircraftCount) {
        return;
    }
    m_lastAircraftCount = count;

    char countLabel[24];
    snprintf(countLabel, sizeof(countLabel), "%u aircraft", count);
    m_manager.fillRect(kCenterX - 52, kSize - 24, 104, 16, kColorBackground);
    m_manager.setLegacyTextColor(kColorLabel, kColorBackground);
    m_manager.setLegacyTextDatum(TC_DATUM);
    m_manager.drawLegacyString(countLabel, kCenterX, kSize - 18, 2);
}

void PlaneRadarWidget::updateAircraftDetail(int screenIndex, int rank, const DetailSnapshot &prev,
                                            const DetailSnapshot &next) {
    m_manager.selectScreen(screenIndex);

    if (rank < 0 || rank >= static_cast<int>(m_sortedCount)) {
        drawEmptyDetail(screenIndex);
        return;
    }

    const AdsbClient::Aircraft &plane = AdsbClient::aircraftList()[m_sorted[rank].index];
    const bool newPlane =
        !prev.occupied || strncmp(prev.callsign, next.callsign, sizeof(prev.callsign)) != 0;

    if (newPlane) {
        m_manager.fillScreen(kColorBackground);
    }

    if (newPlane || next.dbFlags != prev.dbFlags) {
        if (hasSpecialDbFlags(plane.dbFlags)) {
            drawSpecialOrbOutline(m_manager, dbFlagsOutlineColor(plane.dbFlags));
        } else {
            drawSpecialOrbOutline(m_manager, kColorBackground);
        }
    }

    if (newPlane || strncmp(next.callsign, prev.callsign, sizeof(next.callsign)) != 0) {
        m_manager.fillRect(0, 14, kSize, 36, kColorBackground);
        const String callsign = next.callsign[0] != '\0' ? String(next.callsign) : String("Unknown");
        m_manager.drawString(callsign, kCenterX, 32, 22, Align::MiddleCenter, kColorLabel, kColorBackground);
    }

    if (newPlane || strncmp(next.desc, prev.desc, sizeof(next.desc)) != 0) {
        m_manager.fillRect(0, 50, kSize, 40, kColorBackground);
        m_manager.setFontColor(kColorLabel, kColorBackground);
        const String description = next.desc[0] != '\0' ? String(next.desc) : String("Unknown");
        m_manager.drawFittedString(description, kCenterX, 68, 200, 32, Align::MiddleCenter);
    }

    if (newPlane || strncmp(next.route, prev.route, sizeof(next.route)) != 0) {
        m_manager.fillRect(0, 88, kSize, 32, kColorBackground);
        if (next.route[0] != '\0') {
            m_manager.setFontColor(kColorLabel, kColorBackground);
            m_manager.drawFittedString(String(next.route), kCenterX, 102, 200, 28, Align::MiddleCenter);
        }
    }

    if (newPlane || abs(next.speedMph - prev.speedMph) >= kSpeedRedrawThresholdMph) {
        m_manager.fillRect(0, 118, kSize, 30, kColorBackground);
        const String speed = next.speedMph > 0 ? String(next.speedMph) + " mph" : String("— mph");
        m_manager.drawString(speed, kCenterX, 132, 18, Align::MiddleCenter, kColorLabel, kColorBackground);
    }

    if (newPlane || strncmp(next.alt, prev.alt, sizeof(next.alt)) != 0) {
        m_manager.fillRect(0, 150, kSize, 38, kColorBackground);
        const String altitude = next.alt[0] != '\0' ? String(next.alt) : String("—");
        m_manager.drawString(altitude, kCenterX, 164, 18, Align::MiddleCenter, kColorAlt, kColorBackground);
    }

    const MovementHint movement = static_cast<MovementHint>(next.movement);
    const char *movementLabelText = movementLabel(movement);
    const MovementHint prevMovement = static_cast<MovementHint>(prev.movement);
    const char *prevMovementLabelText = movementLabel(prevMovement);
    const bool movementLabelChanged = (movementLabelText == nullptr) != (prevMovementLabelText == nullptr) ||
                                      (movementLabelText != nullptr && prevMovementLabelText != nullptr &&
                                       strcmp(movementLabelText, prevMovementLabelText) != 0);

    if (newPlane || next.dbFlags != prev.dbFlags || movementLabelChanged) {
        m_manager.fillRect(0, 188, kSize, 32, kColorBackground);
        if (hasSpecialDbFlags(plane.dbFlags)) {
            char flagLabel[48];
            formatDbFlagsLabel(plane.dbFlags, flagLabel, sizeof(flagLabel));
            m_manager.drawString(String(flagLabel), kCenterX, 204, 14, Align::MiddleCenter,
                                 dbFlagsOutlineColor(plane.dbFlags), kColorBackground);
        } else if (movementLabelText != nullptr) {
            m_manager.drawString(String(movementLabelText), kCenterX, 204, 14, Align::MiddleCenter,
                                 movementColor(movement), kColorBackground);
        }
    }
}

void PlaneRadarWidget::buildDetailSnapshot(int rank, DetailSnapshot *out) const {
    *out = {};
    if (rank < 0 || rank >= static_cast<int>(m_sortedCount)) {
        return;
    }

    const AdsbClient::Aircraft &plane = AdsbClient::aircraftList()[m_sorted[rank].index];
    strncpy(out->callsign, plane.callsign, sizeof(out->callsign) - 1);
    strncpy(out->desc, plane.desc[0] != '\0' ? plane.desc : plane.type, sizeof(out->desc) - 1);
    RouteClient::formatRoute(plane.callsign, out->route, sizeof(out->route));
    strncpy(out->alt, plane.alt, sizeof(out->alt) - 1);
    out->speedMph = knotsToMph(plane.gsKnots);
    out->dbFlags = plane.dbFlags;
    out->movement = static_cast<int8_t>(classifyMovement(plane));
    out->occupied = true;
}

int PlaneRadarWidget::resolveDetailRank(int slot) const {
    if (m_sortedCount == 0) {
        return -1;
    }

    if (m_slotCallsign[slot][0] != '\0') {
        for (size_t i = 0; i < m_sortedCount; ++i) {
            const char *callsign = AdsbClient::aircraftList()[m_sorted[i].index].callsign;
            if (strncmp(callsign, m_slotCallsign[slot], sizeof(m_slotCallsign[slot])) == 0) {
                return static_cast<int>(i);
            }
        }
    }

    for (size_t i = 0; i < m_sortedCount; ++i) {
        const char *callsign = AdsbClient::aircraftList()[m_sorted[i].index].callsign;
        bool used = false;
        for (int s = 0; s < kDetailSlots; ++s) {
            if (s != slot && m_slotCallsign[s][0] != '\0' &&
                strncmp(m_slotCallsign[s], callsign, sizeof(m_slotCallsign[s])) == 0) {
                used = true;
                break;
            }
        }
        if (!used) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void PlaneRadarWidget::drawEmptyDetail(int screenIndex) {
    m_manager.selectScreen(screenIndex);
    m_manager.fillScreen(kColorBackground);
    m_manager.drawString("No aircraft", kCenterX, kCenterY, 18, Align::MiddleCenter, kColorLabel, kColorBackground);
}

void PlaneRadarWidget::drawChangedDetails() {
    for (int slot = 0; slot < kDetailSlots; ++slot) {
        const int rank = resolveDetailRank(slot);

        DetailSnapshot next{};
        if (rank >= 0) {
            buildDetailSnapshot(rank, &next);
            strncpy(m_slotCallsign[slot], next.callsign, sizeof(m_slotCallsign[slot]) - 1);
        } else {
            m_slotCallsign[slot][0] = '\0';
        }

        const DetailSnapshot &prev = m_lastDetail[slot];
        const MovementHint movement = static_cast<MovementHint>(next.movement);
        const char *movementLabelText = next.occupied ? movementLabel(movement) : nullptr;
        const MovementHint prevMovement = static_cast<MovementHint>(prev.movement);
        const char *prevMovementLabelText = prev.occupied ? movementLabel(prevMovement) : nullptr;
        const bool movementLabelChanged = (movementLabelText == nullptr) != (prevMovementLabelText == nullptr) ||
                                          (movementLabelText != nullptr && prevMovementLabelText != nullptr &&
                                           strcmp(movementLabelText, prevMovementLabelText) != 0);
        const bool speedChanged =
            next.occupied && prev.occupied && abs(next.speedMph - prev.speedMph) >= kSpeedRedrawThresholdMph;
        const bool changed = next.occupied != prev.occupied ||
                             strncmp(next.callsign, prev.callsign, sizeof(next.callsign)) != 0 ||
                             strncmp(next.desc, prev.desc, sizeof(next.desc)) != 0 ||
                             strncmp(next.route, prev.route, sizeof(next.route)) != 0 ||
                             strncmp(next.alt, prev.alt, sizeof(next.alt)) != 0 || speedChanged ||
                             next.dbFlags != prev.dbFlags || movementLabelChanged;

        if (!changed) {
            continue;
        }

        if (next.occupied) {
            updateAircraftDetail(slot + 1, rank, prev, next);
        } else {
            drawEmptyDetail(slot + 1);
        }
        m_lastDetail[slot] = next;
    }
}

void PlaneRadarWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        cycleRange();
    }
}

String PlaneRadarWidget::getName() {
    return "Plane Radar";
}
