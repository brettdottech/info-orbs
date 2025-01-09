#ifndef FEEDFACTORY_H
#define FEEDFACTORY_H

#include "../../core/Feed.h"
#include "DateTimeFeed.h"

class FeedFactory {
public:
  static Feed* CreateFeed(FeedId feedId) { 
    switch (feedId) {
        case FeedDateTime: return new DateTimeFeed();
    }
  }
};

#endif 