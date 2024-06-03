#include "widgets/stockDataModel.h"

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
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_currentPrice;
    return String(stream.str().c_str());
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
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_lowPrice;
    return String(stream.str().c_str());
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
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_highPrice;
    return String(stream.str().c_str());
}

StockDataModel &StockDataModel::setOpenPrice(float openPrice) {
    if (m_openPrice != openPrice) {
        m_openPrice = openPrice;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getOpenPrice() {
    return m_openPrice;
}

String StockDataModel::getOpenPrice(int8_t digits) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_openPrice;
    return String(stream.str().c_str());
}

StockDataModel &StockDataModel::setPreviousClosePrice(float previousClosePrice) {
    if (m_previousClosePrice != previousClosePrice) {
        m_previousClosePrice = previousClosePrice;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getPreviousClosePrice() {
    return m_previousClosePrice;
}

String StockDataModel::getPreviousClosePrice(int8_t digits) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_previousClosePrice;
    return String(stream.str().c_str());
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
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_priceChange;
    return String(stream.str().c_str());
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
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(digits) << m_percentChange;
    return String(stream.str().c_str());
}

bool StockDataModel::isChanged() {
    return m_changed;
}
StockDataModel &StockDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}
