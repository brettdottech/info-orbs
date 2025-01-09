#ifndef FEED_H
#define FEED_H

#include <Arduino.h>

enum FeedId
{
    FeedDateTime = 0,
    FeedWeather = 1,
    FeedStock = 2,
    FeedMQTT = 3
};

struct Feed {
   const FeedId Id;
   
   explicit Feed(FeedId id) : Id(id) {}
   template<class DerivedFeedType> DerivedFeedType getFeed() { return *static_cast<DerivedFeedType*>(this); }
};

#endif