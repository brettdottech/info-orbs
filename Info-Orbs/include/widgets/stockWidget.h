#ifndef STOCK_WIDGET_H
#define STOCK_WIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>

#include "stockDataModel.h"
#include "widget.h"

#define MAX_STOCKS 5

class StockWidget : public Widget {
   public:
    StockWidget(ScreenManager &manager);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void changeMode() override;

   private:
    void getStockData(StockDataModel &stock);
    void displayStock(int8_t displayIndex, StockDataModel &stock, uint32_t backgroundColor, uint32_t textColor);

    String m_apiKey = "c1tjb52ad3ia4h4uee9g";
    unsigned long m_stockDelay = 60000;  //default to 60s between updates
    unsigned long m_stockDelayPrev = 0;

    StockDataModel m_stocks[MAX_STOCKS];
    int8_t m_stockCount;
};
#endif  // STOCK_WIDGET_H
