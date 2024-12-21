// include guard to ensure header is not included more than once
#ifndef TEMPESTFEED_H
#define TEMPESTFEED_H

#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

class TempestFeed : public WeatherFeed {
public:
    TempestFeed(const String &apiKey, const std::string &location, int units);
    bool getWeatherData(WeatherDataModel &model) override;
    String translateIcon(const std::string& icon);
    
private:
    String apiKey;
    std::string location;
    int units;
};

#endif // TEMPESTFEED_H
