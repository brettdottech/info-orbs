#ifndef STOCK_WIDGET_H
#define STOCK_WIDGET_H

#include "widget.h"
#include <vector>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>

class StockWidget : public Widget {
public:
    StockWidget(ScreenManager &manager);
    void setup() override;
    void update() override;
    void update(bool force) override;
    void draw() override;
    void draw(bool force) override;

private:
    void getStockData();
    void displayStock(int displayIndex, uint32_t backgroundColor, uint32_t textColor);

    unsigned long m_stockDelay;
    unsigned long m_stockDelayPrev;
    int currentStockIndex;
    std::vector<String> stockTickers;

    // Stock data variables
    String m_trackingStock;
    String m_currentPrice;
    String m_change;
    String m_percentChange;
    String m_highPrice;
    String m_lowPrice;
    String m_openPrice;
    String m_previousClosePrice;
};

#endif // STOCK_WIDGET_H
