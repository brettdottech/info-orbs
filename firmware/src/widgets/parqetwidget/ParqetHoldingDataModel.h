#ifndef PARQET_HOLDING_DATA_MODEL_H
#define PARQET_HOLDING_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>

class ParqetHoldingDataModel {
public:
    ParqetHoldingDataModel();

    void setId(const String &id);
    void setName(const String &name);
    void setPurchasePrice(float purchasePrice);
    void setPurchaseValue(float purchaseValue);
    void setCurrentPrice(float currentPrice);
    void setCurrentValue(float currentValue);
    void setShares(float shares);
    void setCurrency(const String &currency);
    void setPerformance(float perf);

    String getId() const;
    String getName() const;
    String getPurchasePrice(int8_t digits) const;
    String getPurchaseValue(int8_t digits) const;
    float getCurrentPrice() const;
    String getCurrentPrice(int8_t digits) const;
    float getCurrentValue() const;
    String getCurrentValue(int8_t digits) const;
    String getShares(int8_t digits) const;
    String getCurrency() const;
    float getPerformance() const;
    String getPerformance(int8_t digits) const;

private:
    String m_id = "";
    String m_name = "";
    float m_purchasePrice = 0;
    float m_purchaseValue = 0;
    float m_currentPrice = 0;
    float m_currentValue = 0;
    float m_shares = 0;
    String m_currency = "";
    float m_performance = 0;
};

#endif // PARQET_HOLDING_DATA_MODEL_H
