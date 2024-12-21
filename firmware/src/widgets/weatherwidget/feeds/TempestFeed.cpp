#include "TempestFeed.h"
#include "GlobalTime.h"
#include "config_helper.h"
#include <unordered_map>


TempestFeed::TempestFeed(const String &apiKey, const std::string &stationId, int units)
    : apiKey(apiKey), stationId(stationId), units(units) {}

bool TempestFeed::getWeatherData(WeatherDataModel &model) {

    HTTPClient http;
    String tempUnits = units == 0 ? "c" : "f";

    String httpRequestAddress = "https://swd.weatherflow.com/swd/rest/better_forecast?station_id=" + String(stationId.c_str()) +
                                "&units_temp=" + tempUnits + "&units_wind=mph&units_pressure=mb&units_precip=in&units_distance=mi&api_key=" + apiKey;

    http.begin(httpRequestAddress);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();

        JsonDocument doc;
        JsonDocument filter;
                
        filter["current_conditions"]["air_temperature"] = true;
        filter["current_conditions"]["icon"] = true;
        filter["forecast"]["daily"][0]["air_temp_high"] = true;
        filter["forecast"]["daily"][0]["air_temp_low"] = true;
        filter["forecast"]["daily"][0]["conditions"] = true;
        filter["forecast"]["daily"][0]["icon"] = true;
                
        DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
        
        http.end();

        if (!error | DeserializationError::IncompleteInput) {
            model.setCityName(String(WEATHER_TEMPEST_STATION_NAME));
            model.setCurrentTemperature(doc["current_conditions"]["air_temperature"].as<float>());
            model.setCurrentText(doc["forecast"]["daily"][0]["conditions"].as<String>());

            model.setCurrentIcon(translateIcon(doc["current_conditions"]["icon"]));
            model.setTodayHigh(doc["forecast"]["daily"][0]["air_temp_high"].as<float>());
            model.setTodayLow(doc["forecast"]["daily"][0]["air_temp_low"].as<float>());
            for (int i = 0; i < 3; i++) {
                model.setDayIcon(i, translateIcon(doc["forecast"]["daily"][i]["icon"]));
                model.setDayHigh(i, doc["forecast"]["daily"][i]["air_temp_high"].as<float>());
                model.setDayLow(i, doc["forecast"]["daily"][i]["air_temp_low"].as<float>());
            }
        } else {
            // Handle JSON deserialization error
            Serial.println("Deserialization failed: " + String(error.c_str()));
            return false;
        }
    } else {
        // Handle HTTP request error
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }
    return true;
}


String TempestFeed::translateIcon(const std::string& icon) {
    // Define the mapping of input strings to simplified weather icons
    static const std::unordered_map<std::string, std::string> iconMapping = {
        {"clear-day", "clear-day"},
        {"clear-night", "clear-night"},
        {"partly-cloudy-day", "partly-cloudy-day"},
        {"partly-cloudy-night", "partly-cloudy-night"},
        {"cloudy", "partly-cloudy-day"},  // Map cloudy to partly-cloudy-day
        {"rain", "rain"},
        {"possibly-rain-day", "rain"},
        {"possibly-rain-night", "rain"},
        {"snow", "snow"},
        {"possibly-snow-day", "snow"},
        {"possibly-snow-night", "snow"},
        {"fog", "fog"},
        {"hail", "rain"},  // Example: treat hail as rain
        {"thunderstorm", "rain"},  // Example: treat thunderstorm as rain
        {"wind", "clear-day"},  // Example: map windy to clear-day
        {"tornado", "rain"}  // Example: treat tornado as rain
    };

    // Find the icon in the mapping and return the corresponding simplified value
    auto it = iconMapping.find(icon);
    if (it != iconMapping.end()) {
        return String(it->second.c_str());
    }
    return "clear-day";
}

