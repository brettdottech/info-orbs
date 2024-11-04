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
    // Delete old array
    delete [] m_chartdata;
    // Assign new array
    m_chartdata = chartData;
    m_chartdataCount = count;
}

void ParqetDataModel::clearChartData() {
    Serial.println("clearChartData()");
    setChartData(new float[0], 0);
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

void ParqetDataModel::getChartDataScale(uint8_t maxY, float &scale, float &minVal, float &maxVal, float &chartMinVal) {
    float _minVal = infinityf();
    float _maxVal = -infinityf();
    for (int i=0; i < m_chartdataCount; i++) {
        if (m_chartdata[i] < _minVal)
            _minVal = m_chartdata[i];
        if (m_chartdata[i] > _maxVal)
            _maxVal = m_chartdata[i];            
    }
    float zero = 0;
    // the chart will always show zero, we need to take that into account for our scale
    float delta = max(_maxVal, zero) - min(_minVal, zero);
    if (delta == 0) {
        // avoid Div by zero
        scale = 1;
    } else {
        scale = maxY / delta;
    }
    minVal = _minVal;
    maxVal = _maxVal;
    chartMinVal = min(_minVal, zero);
}