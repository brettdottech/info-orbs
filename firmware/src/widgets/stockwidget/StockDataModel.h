#ifndef STOCK_DATA_MODEL_H
#define STOCK_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>

class StockDataModel {
   public:
    StockDataModel();
    StockDataModel &setCurrencySymbol(String currencySymbol);
    String getCurrencySymbol();
    StockDataModel &setSymbol(String symbol);
    String getSymbol();
    StockDataModel &setTicker(String ticker);
    String getTicker();
    StockDataModel &setCompany(String company);
    String getCompany();
    StockDataModel &setCurrentPrice(float currentPrice);
    float getCurrentPrice();
    String getCurrentPrice(int8_t digits);
    StockDataModel &setHighPrice(float volume);
    float getHighPrice();
    String getHighPrice(int8_t digits);
    StockDataModel &setLowPrice(float volume);
    float getLowPrice();
    String getLowPrice(int8_t digits);
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
    String m_ticker = "";
    String m_company = "";
    String m_currencySymbolGBP = "£";
    String m_currencySymbolEUR = "€";
    String m_currencySymbol = "";
    float m_currentPrice = 0.0;
    float m_volume = 0.0;
    float m_highPrice = 0.0;
    float m_lowPrice = 0.0; 
    float m_priceChange = 0.0;
    float m_percentChange = 0.0;
    bool m_changed = false;
};

#endif  // STOCK_DATA_MODEL_H
