#include "RouteClient.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <cstring>

namespace {

constexpr char kApiBase[] = "https://api.adsbdb.com/v0/callsign/";
constexpr int kConnectAttemptMs = 200;
constexpr unsigned long kRequestTimeoutMs = 8000;
constexpr unsigned long kMinFetchIntervalMs = 4000;
constexpr size_t kCacheSize = 12;

enum class CacheState : uint8_t {
    Empty = 0,
    Found,
    NotFound,
};

struct CacheEntry {
    char callsign[9];
    char originLabel[28];
    char destLabel[28];
    CacheState state = CacheState::Empty;
};

CacheEntry s_cache[kCacheSize];
unsigned long s_lastFetchMs = 0;
size_t s_evictIndex = 0;

void normalizeCallsign(const char *in, char *out, size_t outLen) {
    out[0] = '\0';
    if (outLen == 0 || in == nullptr) {
        return;
    }

    size_t n = strnlen(in, outLen - 1);
    while (n > 0 && in[n - 1] == ' ') {
        --n;
    }
    size_t start = 0;
    while (start < n && in[start] == ' ') {
        ++start;
    }
    n -= start;
    if (n >= outLen) {
        n = outLen - 1;
    }
    for (size_t i = 0; i < n; ++i) {
        char c = in[start + i];
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(c - 'a' + 'A');
        }
        out[i] = c;
    }
    out[n] = '\0';
}

bool callsignLikelyRouteable(const char *callsign) {
    if (callsign[0] == '\0') {
        return false;
    }
    bool hasLetter = false;
    for (size_t i = 0; callsign[i] != '\0'; ++i) {
        const char c = callsign[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            hasLetter = true;
        }
    }
    return hasLetter;
}

CacheEntry *findEntry(const char *callsign) {
    for (size_t i = 0; i < kCacheSize; ++i) {
        if (s_cache[i].state != CacheState::Empty && strcmp(s_cache[i].callsign, callsign) == 0) {
            return &s_cache[i];
        }
    }
    return nullptr;
}

CacheEntry *allocEntry(const char *callsign) {
    CacheEntry *existing = findEntry(callsign);
    if (existing != nullptr) {
        return existing;
    }

    for (size_t i = 0; i < kCacheSize; ++i) {
        if (s_cache[i].state == CacheState::Empty) {
            return &s_cache[i];
        }
    }

    CacheEntry *slot = &s_cache[s_evictIndex];
    s_evictIndex = (s_evictIndex + 1) % kCacheSize;
    slot->state = CacheState::Empty;
    slot->callsign[0] = '\0';
    slot->originLabel[0] = '\0';
    slot->destLabel[0] = '\0';
    return slot;
}

void formatAirportLabel(const JsonObject &airport, char *out, size_t outLen) {
    out[0] = '\0';
    if (outLen == 0 || airport.isNull()) {
        return;
    }

    const char *city =
        airport["municipality"].is<const char *>() ? airport["municipality"].as<const char *>() : nullptr;
    const char *country =
        airport["country_iso_name"].is<const char *>() ? airport["country_iso_name"].as<const char *>() : nullptr;
    const char *iata =
        airport["iata_code"].is<const char *>() ? airport["iata_code"].as<const char *>() : nullptr;

    if (city != nullptr && city[0] != '\0') {
        if (country != nullptr && country[0] != '\0' && strcmp(country, "US") != 0) {
            snprintf(out, outLen, "%s, %s", city, country);
        } else {
            strncpy(out, city, outLen - 1);
            out[outLen - 1] = '\0';
        }
        return;
    }

    if (iata != nullptr && iata[0] != '\0') {
        strncpy(out, iata, outLen - 1);
        out[outLen - 1] = '\0';
    }
}

int performGet(HTTPClient &http) {
    http.setConnectTimeout(kConnectAttemptMs);
    const unsigned long deadline = millis() + kRequestTimeoutMs;
    while (millis() < deadline) {
        const int code = http.GET();
        if (code > 0) {
            return code;
        }
        if (code != HTTPC_ERROR_CONNECTION_REFUSED && code != HTTPC_ERROR_NOT_CONNECTED) {
            return code;
        }
        delay(5);
    }
    return HTTPC_ERROR_READ_TIMEOUT;
}

bool fetchRouteFromApi(const char *callsign, char *originLabel, size_t originLen, char *destLabel,
                       size_t destLen) {
    originLabel[0] = '\0';
    destLabel[0] = '\0';

    String url = kApiBase;
    url += callsign;

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, url)) {
        Serial.println("route: http.begin failed");
        return false;
    }

    http.setTimeout(kRequestTimeoutMs);
    const int code = performGet(http);
    if (code == HTTP_CODE_NOT_FOUND) {
        http.end();
        return true;
    }
    if (code != HTTP_CODE_OK) {
        Serial.printf("route: HTTP %d for %s\n", code, callsign);
        http.end();
        return false;
    }

    const String payload = http.getString();
    http.end();
    if (payload.length() == 0) {
        return false;
    }

    JsonDocument doc;
    const DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        Serial.printf("route: JSON error: %s\n", err.c_str());
        return false;
    }

    JsonObject route = doc["response"]["flightroute"].as<JsonObject>();
    if (route.isNull()) {
        return true;
    }

    formatAirportLabel(route["origin"].as<JsonObject>(), originLabel, originLen);
    formatAirportLabel(route["destination"].as<JsonObject>(), destLabel, destLen);
    return true;
}

} // namespace

void RouteClient::clearCache() {
    for (size_t i = 0; i < kCacheSize; ++i) {
        s_cache[i].state = CacheState::Empty;
        s_cache[i].callsign[0] = '\0';
        s_cache[i].originLabel[0] = '\0';
        s_cache[i].destLabel[0] = '\0';
    }
    s_lastFetchMs = 0;
    s_evictIndex = 0;
}

bool RouteClient::isCached(const char *callsign) {
    char key[9];
    normalizeCallsign(callsign, key, sizeof(key));
    if (!callsignLikelyRouteable(key)) {
        return true;
    }
    const CacheEntry *entry = findEntry(key);
    return entry != nullptr && entry->state != CacheState::Empty;
}

bool RouteClient::formatRoute(const char *callsign, char *out, size_t outLen) {
    out[0] = '\0';
    if (outLen == 0) {
        return false;
    }

    char key[9];
    normalizeCallsign(callsign, key, sizeof(key));
    const CacheEntry *entry = findEntry(key);
    if (entry == nullptr || entry->state != CacheState::Found) {
        return false;
    }
    if (entry->originLabel[0] == '\0' || entry->destLabel[0] == '\0') {
        return false;
    }

    snprintf(out, outLen, "%s -> %s", entry->originLabel, entry->destLabel);
    return true;
}

bool RouteClient::fetchIfUncached(const char *callsign) {
    char key[9];
    normalizeCallsign(callsign, key, sizeof(key));
    if (!callsignLikelyRouteable(key)) {
        return false;
    }
    if (findEntry(key) != nullptr) {
        return false;
    }
    if (s_lastFetchMs != 0 && (millis() - s_lastFetchMs) < kMinFetchIntervalMs) {
        return false;
    }

    char originLabel[28];
    char destLabel[28];
    if (!fetchRouteFromApi(key, originLabel, sizeof(originLabel), destLabel, sizeof(destLabel))) {
        return false;
    }

    s_lastFetchMs = millis();

    CacheEntry *entry = allocEntry(key);
    strncpy(entry->callsign, key, sizeof(entry->callsign) - 1);
    entry->callsign[sizeof(entry->callsign) - 1] = '\0';

    if (originLabel[0] != '\0' && destLabel[0] != '\0') {
        strncpy(entry->originLabel, originLabel, sizeof(entry->originLabel) - 1);
        entry->originLabel[sizeof(entry->originLabel) - 1] = '\0';
        strncpy(entry->destLabel, destLabel, sizeof(entry->destLabel) - 1);
        entry->destLabel[sizeof(entry->destLabel) - 1] = '\0';
        entry->state = CacheState::Found;
        Serial.printf("route: %s %s -> %s\n", key, originLabel, destLabel);
    } else {
        entry->originLabel[0] = '\0';
        entry->destLabel[0] = '\0';
        entry->state = CacheState::NotFound;
        Serial.printf("route: %s not found\n", key);
    }

    return true;
}

bool RouteClient::prefetchOne(const char *const *callsigns, size_t count, size_t *cursor) {
    if (callsigns == nullptr || count == 0 || cursor == nullptr) {
        return false;
    }

    const size_t start = *cursor % count;
    for (size_t attempt = 0; attempt < count; ++attempt) {
        const size_t idx = (start + attempt) % count;
        const char *callsign = callsigns[idx];
        if (callsign == nullptr || callsign[0] == '\0') {
            continue;
        }
        if (isCached(callsign)) {
            continue;
        }
        if (fetchIfUncached(callsign)) {
            *cursor = (idx + 1) % count;
            return true;
        }
        return false;
    }

    return false;
}
