#ifndef ADSB_CLIENT_H
#define ADSB_CLIENT_H

#include <cstddef>
#include <cstdint>

class AdsbClient {
public:
    struct Aircraft {
        float lat;
        float lon;
        float noseDeg;
        float trackDeg;
        float gsKnots;
        char callsign[9];
        char type[5];
        char desc[36];
        char alt[12];
        uint8_t dbFlags = 0;
        int16_t baroRateFpm = 0;
        bool hasBaroRate = false;
    };

    static constexpr size_t kMaxAircraft = 64;

    static bool fetchUpdate(double centerLat, double centerLon, float fetchRadiusKm);
    static size_t aircraftCount();
    static const Aircraft *aircraftList();
};

#endif
