#include "model/parqetDataModel.h"
#include "utils.h"
#include <config.h>

ParqetDataModel::ParqetDataModel() {
}

ParqetDataModel &ParqetDataModel::setPortfolioId(String portfolioId) {
    m_portfolioId = portfolioId;
    return *this;
}

String ParqetDataModel::getPortfolioId() {
    return m_portfolioId;
}

void sortHoldings(ParqetHoldingDataModel* holdings, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (holdings[j].getCurrentValue() < holdings[j + 1].getCurrentValue()) {
                ParqetHoldingDataModel temp = holdings[j];
                holdings[j] = holdings[j + 1];
                holdings[j + 1] = temp;
            }
        }
    }
}

void ParqetDataModel::setHoldings(ParqetHoldingDataModel *holdings, int count) {
    Serial.printf("setHolding() count=%d\n", count);
    // Delete old array
    delete [] m_holdings;
    // Assign new array
    sortHoldings(holdings, count);
    m_holdings = holdings;
    m_holdingsCount = count;
}

ParqetHoldingDataModel &ParqetDataModel::getHolding(int index) {
    return m_holdings[index];
}

int ParqetDataModel::getHoldingsCount() {
    return m_holdingsCount;
}
