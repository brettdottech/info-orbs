#ifndef STOCK_DATA_MODEL_H
#define STOCK_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>
#include <sstream>

class StockDataModel {
   public:
    StockDataModel();
    StockDataModel &setSymbol(String symbol);
    String getSymbol();
    StockDataModel &setCurrentPrice(float currentPrice);
    float getCurrentPrice();
    String getCurrentPrice(int8_t digits);
    StockDataModel &setLowPrice(float lowPrice);
    float getLowPrice();
    String getLowPrice(int8_t digits);
    StockDataModel &setHighPrice(float highPrice);
    float getHighPrice();
    String getHighPrice(int8_t digits);
    StockDataModel &setOpenPrice(float openPrice);
    float getOpenPrice();
    String getOpenPrice(int8_t digits);
    StockDataModel &setPreviousClosePrice(float previousClosePrice);
    float getPreviousClosePrice();
    String getPreviousClosePrice(int8_t digits);
    StockDataModel &setPriceChange(float change);
    float getPriceChange();
    String getPriceChange(int8_t digits);
    StockDataModel &setPercentChange(float percentChange);
    float getPercentChange();
    String getPercentChange(int8_t digits);

    bool isChanged();
    StockDataModel &setChangedStatus(bool changed);

   private:
    String m_symbol = "";
    float m_currentPrice = 0.0;
    float m_lowPrice = 0.0;
    float m_highPrice = 0.0;
    float m_openPrice = 0.0;
    float m_previousClosePrice = 0.0;
    float m_priceChange = 0.0;
    float m_percentChange = 0.0;
    bool m_changed = false;
};

#endif  // STOCK_DATA_MODEL_H