// include guard to ensure header is not included more than once
#ifndef VISUALCROSSINGFEED_H
#define VISUALCROSSINGFEED_H

#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

class VisualCrossingFeed : public WeatherFeed {
public:
    VisualCrossingFeed(const String &apiKey, const std::string &location, int units);
    bool getWeatherData(WeatherDataModel &model) override;

private:
    String apiKey;
    std::string location;
    int units;
};

#endif // VISUALCROSSINGFEED_H
