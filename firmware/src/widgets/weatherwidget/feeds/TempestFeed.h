// include guard to ensure header is not included more than once
#ifndef TEMPESTFEED_H
#define TEMPESTFEED_H

#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

class TempestFeed : public WeatherFeed {
public:
    TempestFeed(const String &apiKey, const String &stationId, int units, const String &stationName);
    bool getWeatherData(WeatherDataModel &model) override;
    String translateIcon(const std::string& icon);
    
private:
    String apiKey;
    String stationId;
    String stationName;
    int units;
};

#endif // TEMPESTFEED_H
