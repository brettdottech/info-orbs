#include "ParqetHoldingDataModel.h"
#include "Utils.h"
#include "config_helper.h"

ParqetHoldingDataModel::ParqetHoldingDataModel() {
}

void ParqetHoldingDataModel::setId(const String &id) {
    m_id = id;
}

void ParqetHoldingDataModel::setName(const String &name) {
    m_name = name;
}

void ParqetHoldingDataModel::setPurchasePrice(const float purchasePrice) {
    m_purchasePrice = purchasePrice;
}

void ParqetHoldingDataModel::setPurchaseValue(const float purchaseValue) {
    m_purchaseValue = purchaseValue;
}

void ParqetHoldingDataModel::setCurrentPrice(const float currentPrice) {
    m_currentPrice = currentPrice;
}

void ParqetHoldingDataModel::setCurrentValue(const float currentValue) {
    m_currentValue = currentValue;
}

void ParqetHoldingDataModel::setShares(const float shares) {
    m_shares = shares;
}

void ParqetHoldingDataModel::setCurrency(const String &currency) {
    m_currency = currency;
}

void ParqetHoldingDataModel::setPerformance(const float perf) {
    m_performance = perf;
}

String ParqetHoldingDataModel::getId() const {
    return m_id;
}

String ParqetHoldingDataModel::getName() const {
    return m_name;
}

String ParqetHoldingDataModel::getPurchasePrice(const int8_t digits) const {
    return Utils::formatFloat(m_purchasePrice, digits);
}

String ParqetHoldingDataModel::getPurchaseValue(const int8_t digits) const {
    return Utils::formatFloat(m_purchaseValue, digits);
}

float ParqetHoldingDataModel::getCurrentPrice() const {
    return m_currentPrice;
}

String ParqetHoldingDataModel::getCurrentPrice(const int8_t digits) const {
    return Utils::formatFloat(m_currentPrice, digits);
}

float ParqetHoldingDataModel::getCurrentValue() const {
    return m_currentValue;
}

String ParqetHoldingDataModel::getCurrentValue(const int8_t digits) const {
    return Utils::formatFloat(m_currentValue, digits);
}

String ParqetHoldingDataModel::getShares(const int8_t digits) const {
    return Utils::formatFloat(m_shares, digits);
}

float ParqetHoldingDataModel::getPerformance() const {
    return m_performance;
}

String ParqetHoldingDataModel::getPerformance(const int8_t digits) const {
    return Utils::formatFloat(m_performance, digits);
}

String ParqetHoldingDataModel::getCurrency() const {
    return m_currency;
}