#ifndef STOCK_SCREEN_WIDGET_H
#define STOCK_SCREEN_WIDGET_H

#include "Widget.h"
#include <vector>

class StockScreenWidget : public Widget {
public:
    StockScreenWidget(ScreenManager &manager);
    void setup() override;
    void draw() override;
    void update() override;

private:
    void getStockData();
    void displayStock(int displayIndex, uint32_t backgroundColor, uint32_t textColor);

    unsigned long m_stockDelayPrev;
    unsigned long m_stockDelay; // Add this line
    int currentStockIndex;
    std::vector<String> stockTickers;

    // Stock data variables
    String m_TrackingStock;
    String currentPrice;
    String change;
    String percentChange;
    String highPrice;
    String lowPrice;
    String openPrice;
    String previousClosePrice;
};

#endif // STOCK_SCREEN_WIDGET_H
