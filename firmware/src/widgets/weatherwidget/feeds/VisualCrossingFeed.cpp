#include "VisualCrossingFeed.h"
#include "../WeatherTranslations.h"
#include "GlobalTime.h"
#include "TaskFactory.h"
#include "config_helper.h"

VisualCrossingFeed::VisualCrossingFeed(const String &apiKey, int units)
    : apiKey(apiKey), m_units(units) {}

void VisualCrossingFeed::setupConfig(ConfigManager &config) {

    config.addConfigString("WeatherWidget", "weatherLocation", &m_weatherLocation, 40, t_weatherLocation);
}

bool VisualCrossingFeed::getWeatherData(WeatherDataModel &model) {

    String tempUnits = m_units == 0 ? "metric" : "us";

    String lang = I18n::getLanguageString();
    if (lang != "en" && lang != "de" && lang != "fr") {
        // Language is not supported on visualcrossing -> use english
        lang = "en";
    }

    String httpRequestAddress = String(WEATHER_VISUALCROSSING_API_URL) +
                                String(m_weatherLocation.c_str()) + "/next4days?key=" + apiKey + "&unitGroup=" + tempUnits +
                                "&include=days,current&iconSet=icons1&lang=" + lang;

    auto task = TaskFactory::createHttpGetTask(
        httpRequestAddress, [this, &model](int httpCode, const String &response) { processResponse(httpCode, response, model); }, [this](int httpCode, String &response) { preProcessResponse(httpCode, response); });

    if (!task) {
        Log.errorln("Failed to create weather task");
        return false;
    }

    bool success = TaskManager::getInstance()->addTask(std::move(task));
    if (!success) {
        Log.errorln("Failed to add weather task");
    }

    return success;
}

void VisualCrossingFeed::preProcessResponse(int httpCode, String &response) {
    if (httpCode > 0) {
        JsonDocument filter;
        filter["resolvedAddress"] = true;
        filter["currentConditions"]["temp"] = true;
        filter["currentConditions"]["icon"] = true;
        // Need days 0-3 for today + 3 day forecast
        filter["days"][0]["description"] = true;
        filter["days"][0]["icon"] = true;
        filter["days"][0]["tempmax"] = true;
        filter["days"][0]["tempmin"] = true;
        filter["days"][1]["icon"] = true;
        filter["days"][1]["tempmax"] = true;
        filter["days"][1]["tempmin"] = true;
        filter["days"][2]["icon"] = true;
        filter["days"][2]["tempmax"] = true;
        filter["days"][2]["tempmin"] = true;
        filter["days"][3]["icon"] = true;
        filter["days"][3]["tempmax"] = true;
        filter["days"][3]["tempmin"] = true;

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response, DeserializationOption::Filter(filter));

        if (!error) {
            response = doc.as<String>();
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    }
}

void VisualCrossingFeed::processResponse(int httpCode, const String &response, WeatherDataModel &model) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

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
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    } else {
        Log.errorln("HTTP request failed, error code: %d", httpCode);
    }
}
