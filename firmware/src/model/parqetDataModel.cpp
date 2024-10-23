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

void ParqetDataModel::setChartData(float* chartData, int count) {
    Serial.printf("setChartData() count=%d\n", count);
    m_chartdata = chartData;
    m_chartdataCount = count;
}

float* ParqetDataModel::getChartData() {
    return m_chartdata;
}

int ParqetDataModel::getHoldingsCount() {
    return m_holdingsCount;
}

int ParqetDataModel::getChartDataCount() {
    return m_chartdataCount;
}

void ParqetDataModel::getChartDataScale(uint8_t maxY, float &scale, float &minY) {
    float minVal = infinityf();
    float maxVal = -infinityf();
    for (int i=0; i < m_chartdataCount; i++) {
        if (m_chartdata[i] < minVal)
            minVal = m_chartdata[i];
        if (m_chartdata[i] > maxVal)
            maxVal = m_chartdata[i];            
    }
    float delta = maxVal - minVal;
    scale = maxY / delta;
    minY = minVal;
}