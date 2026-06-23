#ifndef ROUTE_CLIENT_H
#define ROUTE_CLIENT_H

#include <cstddef>

class RouteClient {
public:
    static void clearCache();
    static bool isCached(const char *callsign);
    static bool formatRoute(const char *callsign, char *out, size_t outLen);
    static bool fetchIfUncached(const char *callsign);
    static bool prefetchOne(const char *const *callsigns, size_t count, size_t *cursor);
};

#endif
