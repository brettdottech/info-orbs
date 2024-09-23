#include "model/stockDataModel.h"
#include "utils.h"
#include <config.h>

StockDataModel::StockDataModel() {
}

StockDataModel &StockDataModel::setSymbol(String symbol) {
    if (m_symbol != symbol) {
        m_symbol = String(symbol);
        // this is not a regular data field so do not mark changed when set
    }
    return *this;
}
String StockDataModel::getSymbol() {
    return m_symbol;
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

StockDataModel &StockDataModel::setVolume(float volume) {
    if (m_volume != volume) {
        m_volume = volume;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getVolume() {
    return m_volume;
}

String StockDataModel::getVolume(int8_t digits) {
    return Utils::formatFloat(m_volume, digits);
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
