#ifndef FEEDS_H
#define FEEDS_H

#include <Arduino.h>
#include "Feed.h"
#include "..\..\feeds\FeedFactory.h"

struct Feeds {
private:
  Feed* _Feeds[MAXFEEDID];
public:
  Feed& registerFeed(const FeedId feedId) {
    if (!_Feeds[feedId])
    {
        _Feeds[feedId] = FeedFactory::CreateFeed(feedId);
    }
  }

  Feed& getFeed(const FeedId feedId) {
    assert(_Feeds[feedId]);
    return *_Feeds[feedId];
  }
};

#endif