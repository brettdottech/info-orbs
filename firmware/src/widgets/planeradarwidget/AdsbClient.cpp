#include "AdsbClient.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <cmath>
#include <cstring>

namespace {

constexpr char kApiBase[] = "https://opendata.adsb.fi/api/v3/lat/";
constexpr float kKmPerNm = 1.852f;
constexpr int kConnectAttemptMs = 200;
constexpr unsigned long kRequestTimeoutMs = 10000;

AdsbClient::Aircraft s_aircraft[AdsbClient::kMaxAircraft];
size_t s_aircraftCount = 0;

int performGetWithRetry(HTTPClient &http) {
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

bool readJsonFloat(const JsonObject &obj, const char *key, float *out) {
    if (obj[key].is<float>() || obj[key].is<double>() || obj[key].is<int>()) {
        *out = obj[key].as<float>();
        return true;
    }
    return false;
}

float pickNoseHeading(const JsonObject &plane) {
    float v = 0.0f;
    if (readJsonFloat(plane, "true_heading", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "mag_heading", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "track", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "dir", &v)) {
        return v;
    }
    return 0.0f;
}

float pickTrackHeading(const JsonObject &plane) {
    float v = 0.0f;
    if (readJsonFloat(plane, "track", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "true_heading", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "mag_heading", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "dir", &v)) {
        return v;
    }
    return 0.0f;
}

float pickGroundSpeed(const JsonObject &plane) {
    float v = 0.0f;
    if (readJsonFloat(plane, "gs", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "tas", &v)) {
        return v;
    }
    if (readJsonFloat(plane, "ias", &v)) {
        return v;
    }
    return 0.0f;
}

bool isOnGround(const JsonObject &plane) {
    if (!plane["alt_baro"].is<const char *>()) {
        return false;
    }
    return strcmp(plane["alt_baro"].as<const char *>(), "ground") == 0;
}

void copyJsonStringTrimmed(const JsonObject &obj, const char *key, char *out, size_t outLen) {
    out[0] = '\0';
    if (outLen == 0 || !obj[key].is<const char *>()) {
        return;
    }
    const char *s = obj[key].as<const char *>();
    size_t n = strnlen(s, outLen - 1);
    while (n > 0 && s[n - 1] == ' ') {
        --n;
    }
    memcpy(out, s, n);
    out[n] = '\0';
}

void formatAltitudeTag(const JsonObject &plane, char *out, size_t outLen) {
    out[0] = '\0';
    if (outLen == 0) {
        return;
    }

    if (plane["alt_baro"].is<const char *>()) {
        const char *s = plane["alt_baro"].as<const char *>();
        if (strcmp(s, "ground") == 0) {
            strncpy(out, "GND", outLen - 1);
            out[outLen - 1] = '\0';
            return;
        }
    }

    float alt = 0.0f;
    if (readJsonFloat(plane, "alt_baro", &alt) || readJsonFloat(plane, "alt_geom", &alt)) {
        snprintf(out, outLen, "%d ft", static_cast<int>(lroundf(alt)));
    }
}

void fillTagFields(AdsbClient::Aircraft *ac, const JsonObject &plane) {
    copyJsonStringTrimmed(plane, "flight", ac->callsign, sizeof(ac->callsign));
    if (ac->callsign[0] == '\0') {
        copyJsonStringTrimmed(plane, "hex", ac->callsign, sizeof(ac->callsign));
    }
    copyJsonStringTrimmed(plane, "t", ac->type, sizeof(ac->type));
    copyJsonStringTrimmed(plane, "desc", ac->desc, sizeof(ac->desc));
    formatAltitudeTag(plane, ac->alt, sizeof(ac->alt));

    ac->dbFlags = 0;
    if (plane["dbFlags"].is<int>() || plane["dbFlags"].is<unsigned int>()) {
        ac->dbFlags = static_cast<uint8_t>(plane["dbFlags"].as<unsigned>() & 0xFF);
    }

    ac->baroRateFpm = 0;
    ac->hasBaroRate = false;
    float baroRate = 0.0f;
    if (readJsonFloat(plane, "baro_rate", &baroRate)) {
        ac->baroRateFpm = static_cast<int16_t>(lroundf(baroRate));
        ac->hasBaroRate = true;
    } else if (readJsonFloat(plane, "geom_rate", &baroRate)) {
        ac->baroRateFpm = static_cast<int16_t>(lroundf(baroRate));
        ac->hasBaroRate = true;
    }
}

} // namespace

bool AdsbClient::fetchUpdate(double centerLat, double centerLon, float fetchRadiusKm) {
    const float distNm = fetchRadiusKm / kKmPerNm;

    String url = kApiBase;
    url += String(centerLat, 6);
    url += "/lon/";
    url += String(centerLon, 6);
    url += "/dist/";
    url += String(distNm, 1);

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, url)) {
        Serial.println("adsb: http.begin failed");
        return false;
    }

    http.setTimeout(kRequestTimeoutMs);
    const int code = performGetWithRetry(http);
    if (code != HTTP_CODE_OK) {
        Serial.printf("adsb: HTTP %d\n", code);
        http.end();
        return false;
    }

    const String payload = http.getString();
    http.end();
    if (payload.length() == 0) {
        Serial.println("adsb: empty response");
        return false;
    }

    JsonDocument doc;
    const DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        Serial.printf("adsb: JSON parse error: %s\n", err.c_str());
        return false;
    }

    JsonArray ac = doc["ac"].as<JsonArray>();
    if (ac.isNull()) {
        s_aircraftCount = 0;
        return true;
    }

    size_t n = 0;
    for (JsonObject plane : ac) {
        if (n >= kMaxAircraft) {
            break;
        }
        if (!plane["lat"].is<float>() || !plane["lon"].is<float>()) {
            continue;
        }
        if (isOnGround(plane)) {
            continue;
        }

        s_aircraft[n].lat = plane["lat"].as<float>();
        s_aircraft[n].lon = plane["lon"].as<float>();
        s_aircraft[n].noseDeg = pickNoseHeading(plane);
        s_aircraft[n].trackDeg = pickTrackHeading(plane);
        s_aircraft[n].gsKnots = pickGroundSpeed(plane);
        fillTagFields(&s_aircraft[n], plane);
        ++n;
    }

    s_aircraftCount = n;
    Serial.printf("adsb: %u aircraft\n", static_cast<unsigned>(n));
    return true;
}

size_t AdsbClient::aircraftCount() {
    return s_aircraftCount;
}

const AdsbClient::Aircraft *AdsbClient::aircraftList() {
    return s_aircraft;
}
