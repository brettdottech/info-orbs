#include "ParqetHoldingDataModel.h"
#include "Utils.h"
#include "config_helper.h"

ParqetHoldingDataModel::ParqetHoldingDataModel() {
}

void ParqetHoldingDataModel::setId(String id) {
    m_id = id;
}
void ParqetHoldingDataModel::setName(String name) {
    m_name = name;
}
void ParqetHoldingDataModel::setPurchasePrice(float purchasePrice) {
    m_purchasePrice = purchasePrice;
}
void ParqetHoldingDataModel::setPurchaseValue(float purchaseValue) {
    m_purchaseValue = purchaseValue;
}
void ParqetHoldingDataModel::setCurrentPrice(float currentPrice) {
    m_currentPrice = currentPrice;
}
void ParqetHoldingDataModel::setCurrentValue(float currentValue) {
    m_currentValue = currentValue;
}
void ParqetHoldingDataModel::setShares(float shares) {
    m_shares = shares;
}
void ParqetHoldingDataModel::setCurrency(String currency) {
    m_currency = currency;
}

void ParqetHoldingDataModel::setPerf(float perf) {
    m_perf = perf;
}

String ParqetHoldingDataModel::getId() {
    return m_id;
}
String ParqetHoldingDataModel::getName() {
    return m_name;
}
String ParqetHoldingDataModel::getPurchasePrice(int8_t digits) {
    return Utils::formatFloat(m_purchasePrice, digits);
}
String ParqetHoldingDataModel::getPurchaseValue(int8_t digits) {
    return Utils::formatFloat(m_purchaseValue, digits);
}
float ParqetHoldingDataModel::getCurrentPrice() {
    return m_currentPrice;
}
String ParqetHoldingDataModel::getCurrentPrice(int8_t digits) {
    return Utils::formatFloat(m_currentPrice, digits);
}
float ParqetHoldingDataModel::getCurrentValue() {
    return m_currentValue;
}
String ParqetHoldingDataModel::getCurrentValue(int8_t digits) {
    return Utils::formatFloat(m_currentValue, digits);
}
String ParqetHoldingDataModel::getShares(int8_t digits) {
    return Utils::formatFloat(m_shares, digits);
}
float ParqetHoldingDataModel::getPerf() {
    return m_perf;
}
String ParqetHoldingDataModel::getPerf(int8_t digits) {
    return Utils::formatFloat(m_perf, digits);
}
String ParqetHoldingDataModel::getCurrency() {
    return m_currency;
}