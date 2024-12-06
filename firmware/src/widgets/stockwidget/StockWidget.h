#ifndef STOCK_WIDGET_H
#define STOCK_WIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "StockDataModel.h"
#include "Widget.h"

#define MAX_STOCKS 5

void taskGetStockData(void *pvParameters);

class StockWidget : public Widget {
public:
    StockWidget(ScreenManager &manager);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

    void changeMode();

private:
    void getStockData(StockDataModel &stock);
    void displayStock(int8_t displayIndex, StockDataModel &stock, uint32_t backgroundColor, uint32_t textColor);

    unsigned long m_stockDelay = 900000; // default to 15m between updates
    unsigned long m_stockDelayPrev = 0;

    StockDataModel m_stocks[MAX_STOCKS];
    int8_t m_stockCount;

    friend void taskGetStockData(void *pvParameters);
};

#endif // STOCK_WIDGET_H
