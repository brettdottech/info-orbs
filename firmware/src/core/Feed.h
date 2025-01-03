#ifndef FEED_H
#define FEED_H

#include <Arduino.h>

enum FeedId
{
    FeedDateTime = 0,
    FeedWeather = 1,
    FeedStock = 2,
    FeedMQTT = 3,
    MAXFEEDID = 4
};

struct Feed {
public:
   virtual String& getName();
   virtual FeedId getId();
   template<class DerivedFeedType> DerivedFeedType getFeed() { return *dynamic_cast<DerivedFeedType*>(this); }
};

#endif