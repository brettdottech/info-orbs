#ifndef ORB_H
#define ORB_H

#include <Arduino.h>
#include "Settings.h"
#include "Feeds.h"
#include "Renderer.h"

enum OrbId
{
    OrbDateTimeWeekday = 1,
    OrbCityWeatherPrediction = 2,
    OrbWeatherIcon = 3,
    OrbTemperatureHighLow = 4,
    OrbWeather3DayForecast = 5
};

struct Orb {
public:
    Orb(const Settings& settings);
    virtual FeedId getFeedId();
    virtual void draw(Renderer& renderer, Settings& settings, Feed& feed);
};

#endif