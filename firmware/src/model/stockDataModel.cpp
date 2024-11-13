#include "model/stockDataModel.h"
#include "utils.h"
#include <config_helper.h>

StockDataModel::StockDataModel() {
}

StockDataModel &StockDataModel::setCurrencySymbol(String currencySymbol) {
        if (currencySymbol == "EUR"){
            m_currencySymbol = m_currencySymbolEUR;
        } else if (currencySymbol == "GBP"){
            m_currencySymbol = m_currencySymbolGBP ;
        } else if (getSymbol().indexOf("/EUR") != -1){
            m_currencySymbol = m_currencySymbolEUR;
        } else if (getSymbol().indexOf("/GBP") != -1){
            m_currencySymbol = m_currencySymbolGBP;
        } else {
            m_currencySymbol =  "$";
        }
    return *this;
}

String StockDataModel::getCurrencySymbol() {
    return m_currencySymbol;
}

StockDataModel &StockDataModel::setSymbol(String symbol) {
    if (m_symbol != symbol) {
        m_symbol = String(symbol);
        // This is not a regular data field so do not mark changed when set
    }
    return *this;
}
String StockDataModel::getSymbol() {
    return m_symbol;
}

StockDataModel &StockDataModel::setTicker(String ticker) {
    if (m_ticker != ticker) {
        m_ticker = String(ticker);
        // This is not a regular data field so do not mark changed when set
    }
    return *this;
}
String StockDataModel::getTicker() {
    return m_ticker;
}


StockDataModel &StockDataModel::setCompany(String company) {
    if (m_company != company) {
        m_company = company;
        m_changed = true;
    }
    return *this;
}
String StockDataModel::getCompany() {
    return m_company;
}
StockDataModel &StockDataModel::setCurrentPrice(float currentPrice) {
    if (m_currentPrice != currentPrice) {
        m_currentPrice = currentPrice;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getCurrentPrice() {
    return m_currentPrice;
}
String StockDataModel::getCurrentPrice(int8_t digits) {
    return Utils::formatFloat(m_currentPrice, digits);
}

StockDataModel &StockDataModel::setHighPrice(float highPrice) {
    if (m_highPrice != highPrice) {
        m_highPrice = highPrice;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getHighPrice() {
    return m_highPrice;
}

String StockDataModel::getHighPrice(int8_t digits) {
    return Utils::formatFloat(m_highPrice, digits);
}



StockDataModel &StockDataModel::setLowPrice(float lowPrice) {
    if (m_lowPrice != lowPrice) {
        m_lowPrice = lowPrice;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getLowPrice() {
    return m_lowPrice;
}

String StockDataModel::getLowPrice(int8_t digits) {
    return Utils::formatFloat(m_lowPrice, digits);
}


StockDataModel &StockDataModel::setPriceChange(float priceChange) {
    if (m_priceChange != priceChange) {
        m_priceChange = priceChange;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getPriceChange() {
    return m_priceChange;
}
String StockDataModel::getPriceChange(int8_t digits) {
    return Utils::formatFloat(m_priceChange, digits);
}

StockDataModel &StockDataModel::setPercentChange(float percentChange) {
    if (m_percentChange != percentChange) {
        m_percentChange = percentChange;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getPercentChange() {
    return m_percentChange;
}

String StockDataModel::getPercentChange(int8_t digits) {
    return Utils::formatFloat(m_percentChange*100, digits);
}

bool StockDataModel::isChanged() {
    return m_changed;
}
StockDataModel &StockDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}
