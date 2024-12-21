// include guard to ensure header is not included more than once
#ifndef WEATHERFEED_H
#define WEATHERFEED_H

#include "WeatherDataModel.h"

class WeatherFeed {
public:
    virtual bool getWeatherData(WeatherDataModel &model) = 0;
};

#endif // WEATHERFEED_H
