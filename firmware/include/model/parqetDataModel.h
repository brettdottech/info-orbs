#ifndef PARQET_DATA_MODEL_H
#define PARQET_DATA_MODEL_H

#include <Arduino.h>
#include <iomanip>

#include "parqetHoldingDataModel.h"

class ParqetDataModel {
   public:
    ParqetDataModel();
    ParqetDataModel &setPortfolioId(String portfolioId);
    String getPortfolioId();
    void setHoldings(ParqetHoldingDataModel *holdings, int count);
    ParqetHoldingDataModel &getHolding(int index);
    int getHoldingsCount();


   private:
    String m_portfolioId = "";
    ParqetHoldingDataModel* m_holdings = nullptr;
    int m_holdingsCount = 0;

};

#endif  // PARQET_DATA_MODEL_H
