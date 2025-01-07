#ifndef STOCK_WIDGET_H
#define STOCK_WIDGET_H

#include <ArduinoJson.h>
#include <HTTPClientWrapper.h>
#include <TFT_eSPI.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "StockDataModel.h"
#include "Widget.h"
#include "config_helper.h"

#define MAX_STOCKS 5

class StockWidget : public Widget {
public:
    StockWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

    void changeMode();

private:
    void getStockData(StockDataModel &stock);
    static void taskGetStockData(void *pvParameters);
    void processResponse(StockDataModel &stock, int httpCode, const String &response);
    void displayStock(int8_t displayIndex, StockDataModel &stock, uint32_t backgroundColor, uint32_t textColor);

    unsigned long m_stockDelay = 900000; // default to 15m between updates
    unsigned long m_stockDelayPrev = 0;

#ifdef STOCK_TICKER_LIST
    std::string m_stockList = STOCK_TICKER_LIST;
#else
    std::string m_stockList = "";
#endif

    StockDataModel m_stocks[MAX_STOCKS];
    int8_t m_stockCount;

#ifndef STOCK_CHANGE_FORMAT
    #define STOCK_CHANGE_FORMAT 0
#endif

    int m_stockchangeformat = STOCK_CHANGE_FORMAT; // Show percent change (0) or price change (1) for stocks
};

#endif // STOCK_WIDGET_H
