#ifndef ORBFACTORY_H
#define ORBFACTORY_H

#include "../../core/Settings.h"
#include "../../core/Orb.h"
#include "DateTimeWeekDayOrb.h"

struct OrbFactory {
    static Orb* CreateOrb(OrbId orbId, const Settings& settings, Feeds* feeds)
    {
        Orb* newOrb;
        switch (orbId)
        {
            case OrbDateTimeWeekday: newOrb = new DateTimeWeekdayOrb(settings);
        }

        feeds->registerFeed(newOrb->getFeedId());
        return newOrb;
    }
};

#endif