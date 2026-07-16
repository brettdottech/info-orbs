#ifndef TEMPESTFEED_H
#define TEMPESTFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>
#include <string> // Include for std::string

#ifndef TEMPEST_PROXY_URL
    #define TEMPEST_PROXY_URL "https://info-orbs-proxy.onrender.com/tempest/proxy"
#endif
#ifndef WEATHER_TEMPEST_STATION_ID
    #define WEATHER_TEMPEST_STATION_ID "136542" // just a default, change to your station ID
#endif
#ifndef WEATHER_TEMPEST_STATION_NAME
    #define WEATHER_TEMPEST_STATION_NAME "Chicago" // just a default, change to your station name
#endif

class TempestFeed : public WeatherFeed {
public:
    TempestFeed(const String &apiKey, int units);
    bool getWeatherData(WeatherDataModel &model) override;
    void setupConfig(ConfigManager &config) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);
    String translateIcon(const std::string &icon);

private:
    String apiKey;
    std::string m_stationId = WEATHER_TEMPEST_STATION_ID;
    std::string m_stationName = WEATHER_TEMPEST_STATION_NAME;
    std::string m_proxyUrl = TEMPEST_PROXY_URL;

    int m_units;
};
#endif
