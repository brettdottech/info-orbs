#include "model/stockDataModel.h"
#include "utils.h"
#include <config.h>

StockDataModel::StockDataModel() {
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




StockDataModel &StockDataModel::setCompany(String company) {
    if (m_company != company) {
        m_company = String(company);
        m_changed = true;
    }
    return *this;
}
String StockDataModel::getCompany() {
    return m_company;
}


StockDataModel &StockDataModel::setCurrency(String currency) {
    if (m_currency != currency) {
        m_currency = String(currency);
        m_changed = true;
    }
    return *this;
}
String StockDataModel::getCurrency() {
    return m_currency;
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

StockDataModel &StockDataModel::setHP(float HP) {
    if (m_HP != HP) {
        m_HP = HP;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getHP() {
    return m_HP;
}

String StockDataModel::getHP(int8_t digits) {
    return Utils::formatFloat(m_HP, digits);
}



StockDataModel &StockDataModel::setLP(float LP) {
    if (m_LP != LP) {
        m_LP = LP;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getLP() {
    return m_LP;
}

String StockDataModel::getLP(int8_t digits) {
    return Utils::formatFloat(m_LP, digits);
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