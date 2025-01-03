#ifndef DATETIMEFEED_H
#define DATETIMEFEED_H

#include "GlobalTime.h"
#include "..\..\core\Feed.h"

class DateTimeFeed : private Feed {
private:
    GlobalTime* _DateTime = nullptr;
public:    
    DateTimeFeed() { _DateTime = GlobalTime::getInstance(); }
    GlobalTime* getData() { return _DateTime; }
};

#endif