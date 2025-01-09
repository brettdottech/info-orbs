#ifndef FEEDS_H
#define FEEDS_H

#include <Arduino.h>
#include <map>
#include "Feed.h"
#include "../../feeds/FeedFactory.h"

struct Feeds {
private:
  std::map<FeedId, Feed*> _Feeds;
public:
  Feed& registerFeed(const FeedId feedId) {
    if (_Feeds.find(feedId) == _Feeds.end())
    {
        auto newFeed = FeedFactory::CreateFeed(feedId);
        _Feeds.insert(std::make_pair(feedId, newFeed));
    }
    return *_Feeds[feedId];
  }

  Feed& getFeed(const FeedId feedId) {
    assert(_Feeds[feedId]);
    return *_Feeds[feedId];
  }
};

#endif