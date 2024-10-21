#include "model/parqetHoldingDataModel.h"
#include "utils.h"
#include <config.h>

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
float ParqetHoldingDataModel::getPercentChange() {
    // return 100 * (m_currentPrice-m_purchasePrice)/m_purchasePrice;
    return 100 * (m_currentValue/m_purchaseValue - 1);
}
String ParqetHoldingDataModel::getPercentChange(int8_t digits) {
    return Utils::formatFloat(getPercentChange(), digits);
}
String ParqetHoldingDataModel::getCurrency() {
    return m_currency;
}