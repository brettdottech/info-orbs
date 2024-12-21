#include "VisualCrossingFeed.h"
#include "GlobalTime.h"
#include "config_helper.h"

VisualCrossingFeed::VisualCrossingFeed(const String &apiKey, const std::string &location, int units)
    : apiKey(apiKey), location(location), units(units) {}

bool VisualCrossingFeed::getWeatherData(WeatherDataModel &model) {
    
    Serial.println("Getting weather data from Visual Crossing");
    
    HTTPClient http;
    String weatherUnits = units == 0 ? "metric" : "us";
    String httpRequestAddress = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                                String(location.c_str()) + "/next3days?key=" + apiKey + "&unitGroup=" + weatherUnits +
                                "&include=days,current&iconSet=icons1&lang=" + LOC_LANG;
    http.begin(httpRequestAddress);
    int httpCode = http.GET();
    if (httpCode > 0) {
        // Check for the return code
        JsonDocument filter;
        filter["resolvedAddress"] = true;
        filter["currentConditions"]["temp"] = true;
        filter["days"][0]["description"] = true;
        filter["currentConditions"]["icon"] = true;
        filter["days"][0]["icon"] = true;
        filter["days"][0]["tempmax"] = true;
        filter["days"][0]["tempmin"] = true;

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
        http.end();

        if (!error) {
            model.setCityName(doc["resolvedAddress"].as<String>());
            model.setCurrentTemperature(doc["currentConditions"]["temp"].as<float>());
            model.setCurrentText(doc["days"][0]["description"].as<String>());

            model.setCurrentIcon(doc["currentConditions"]["icon"].as<String>());
            model.setTodayHigh(doc["days"][0]["tempmax"].as<float>());
            model.setTodayLow(doc["days"][0]["tempmin"].as<float>());
            for (int i = 0; i < 3; i++) {
                model.setDayIcon(i, doc["days"][i + 1]["icon"].as<String>());
                model.setDayHigh(i, doc["days"][i + 1]["tempmax"].as<float>());
                model.setDayLow(i, doc["days"][i + 1]["tempmin"].as<float>());
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
