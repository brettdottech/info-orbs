#ifndef PARQET_WIDGET_H
#define PARQET_WIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <globalTime.h>

#include "model/parqetDataModel.h"
#include "widget.h"
#include "utils.h"

#define MODE_COUNT 3
#define MAX_STOCKNAME_LINES 3

class ParqetWidget : public Widget {
   public:
    ParqetWidget(ScreenManager &manager);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void changeMode() override;
    void changeModeLongpress() override;

   private:
    String getTimeframe();
    void updatePortfolio();
    void displayStock(int8_t displayIndex, ParqetHoldingDataModel &stock, uint32_t backgroundColor, uint32_t textColor);
    ParqetDataModel getPortfolio();
    void clearScreen(int8_t displayIndex, int32_t background);
    void displayClock(int8_t displayIndex, uint32_t background, uint32_t color, String extra);
    
    GlobalTime* m_time;

    unsigned long m_stockDelay = 15*60*1000;  //default to 15m between updates
    unsigned long m_stockDelayPrev = 0;

    unsigned long m_cycleDelay = 10*1000;  // cycle through pages (for more than 4/5 stocks) every 10 seconds
    unsigned long m_cycleDelayPrev = 0;

    unsigned long m_clockDelay = 60*1000;  // update the clock every minute
    unsigned long m_clockDelayPrev = 0;

    String m_modes[MODE_COUNT] = {"today", "ytd", "max"}; // Others like 1y, 5y... are also available
    int8_t m_curMode = 0;

    boolean m_showClock = true; // Show clock on first screen
    boolean m_showTotal = true; // Show your total portfolio value
    boolean m_showValues = false; // Show current price (false) or value in portfolio (true)

    ParqetDataModel m_portfolio;
    int m_holdingsDisplayFrom = 0;
    boolean m_changed = false;

};
#endif  // PARQET_WIDGET_H
