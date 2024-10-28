#ifndef STOCK_DATA_MODEL_H
#define STOCK_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>

class StockDataModel {
   public:
    StockDataModel();
    StockDataModel &setSymbol(String symbol);
    String getSymbol();
    StockDataModel &setCompany(String company);
    String getCompany();
    StockDataModel &setCurrency(String company);
    String getCurrency();
    StockDataModel &setCurrentPrice(float currentPrice);
    float getCurrentPrice();
    String getCurrentPrice(int8_t digits);
    StockDataModel &setHP(float volume);
    float getHP();
    String getHP(int8_t digits);
    StockDataModel &setLP(float volume);
    float getLP();
    String getLP(int8_t digits);
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
    String m_company = "";
    String m_currency = "";
    float m_currentPrice = 0.0;
    float m_volume = 0.0;
    float m_HP = 0.0;
    float m_LP = 0.0; 
    float m_priceChange = 0.0;
    float m_percentChange = 0.0;
    bool m_changed = false;
};

#endif  // STOCK_DATA_MODEL_H
