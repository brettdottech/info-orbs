#ifndef DATETIMEWEEKDAYORB_H
#define DATETIMEWEEKDAYORB_H

#include "GlobalTime.h"
#include "..\..\feeds\DateTimeFeed.h"
#include "..\..\core\Settings.h"
#include "..\..\core\Orb.h"

class DateTimeWeekdayOrb : public Orb {
public:    
    DateTimeWeekdayOrb(const Settings& settings) : Orb(settings) {}

    FeedId getFeedId() { return FeedId::FeedDateTime; }

    void draw(Renderer& renderer, Settings& settings, Feed& feed) override {
        const int mainClockFontSize = 66;
        const int clockY = 120;
        const int dayOfWeekY = 190;
        const int dateY = 50;

        auto dt = feed.getFeed<DateTimeFeed>().getData();

        renderer.selectFont(DEFAULT_FONT);
        renderer.clearScreen();
        renderer.text(renderer.getCenterX(), dateY, 18, Align::MiddleCenter, dt->getDayAndMonth());
        renderer.text(renderer.getCenterX(), dayOfWeekY, 22, Align::MiddleCenter, dt->getWeekday());

        renderer.text(renderer.getCenterX() - 10, clockY, mainClockFontSize, Align::MiddleRight, dt->getHourPadded());
        renderer.text(renderer.getCenterX(), clockY, mainClockFontSize, Align::MiddleCenter, ":");
        renderer.text(renderer.getCenterX() + 10, clockY, mainClockFontSize, Align::MiddleLeft, dt->getMinutePadded());
    }  
};

#endif