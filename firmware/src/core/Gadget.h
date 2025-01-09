#ifndef GADGET_H
#define GADGET_H

#include <Arduino.h>
#include "Settings.h"
#include "Feeds.h"
#include "Orb.h"
#include "../../orbs\OrbFactory.h"


#define ORB_COUNT 5

struct Gadget {
private:
  Feeds _Feeds;  
  Orb* _Orbs[ORB_COUNT];
public:
  Gadget(const Settings& settings) {
    // TODO: get from settings, natch
    _Orbs[0] = OrbFactory::CreateOrb(OrbDateTimeWeekday, settings, _Feeds);
    _Orbs[1] = OrbFactory::CreateOrb(OrbDateTimeWeekday, settings, _Feeds);
    _Orbs[2] = OrbFactory::CreateOrb(OrbDateTimeWeekday, settings, _Feeds);
    _Orbs[3] = OrbFactory::CreateOrb(OrbDateTimeWeekday, settings, _Feeds);
    _Orbs[4] = OrbFactory::CreateOrb(OrbDateTimeWeekday, settings, _Feeds);
  }

};

#endif
