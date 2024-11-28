#ifndef PARQET_HOLDING_DATA_MODEL_H
#define PARQET_HOLDING_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>

class ParqetHoldingDataModel {
   public:
    ParqetHoldingDataModel();

    void setId(String id);
    void setName(String name);
    void setPurchasePrice(float purchasePrice);
    void setPurchaseValue(float purchaseValue);
    void setCurrentPrice(float currentPrice);
    void setCurrentValue(float currentValue);
    void setShares(float shares);
    void setCurrency(String currency);

    String getId();
    String getName();
    String getPurchasePrice(int8_t digits);
    String getPurchaseValue(int8_t digits);
    float getCurrentPrice();
    String getCurrentPrice(int8_t digits);
    float getCurrentValue();
    String getCurrentValue(int8_t digits);
    String getShares(int8_t digits);
    float getPercentChange();
    String getPercentChange(int8_t digits);
    String getCurrency();

   private:
    String m_id = "";
    String m_name = "";
    float m_purchasePrice = 0;
    float m_purchaseValue = 0;
    float m_currentPrice = 0;
    float m_currentValue = 0;
    float m_shares = 0;
    String m_currency = "";
    
};

#endif  // PARQET_HOLDING_DATA_MODEL_H
