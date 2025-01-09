#ifndef DATETIMEFEED_H
#define DATETIMEFEED_H

#include "GlobalTime.h"
#include "../../core/Feed.h"

class DateTimeFeed : public Feed {
private:
    GlobalTime* _DateTime = nullptr;
public:    
    DateTimeFeed() : Feed(FeedId::FeedDateTime) { _DateTime = GlobalTime::getInstance(); }
    GlobalTime* getData() { return _DateTime; }
};

#endif