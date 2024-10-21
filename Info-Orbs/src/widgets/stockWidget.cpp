#include "widgets/stockWidget.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <config.h>

#include <iomanip>

StockWidget::StockWidget(ScreenManager &manager) : Widget(manager) {
    char stockList[strlen(STOCK_TICKER_LIST) + 1];
    strcpy(stockList, STOCK_TICKER_LIST);

    char *symbol = strtok(stockList, ",");
    m_stockCount = 0;
    do {
        StockDataModel stockModel = StockDataModel();
        stockModel.setSymbol(String(symbol));
        m_stocks[m_stockCount] = stockModel;
        m_stockCount++;
        if (m_stockCount > MAX_STOCKS) {
            Serial.println("MAX STOCKS UNABLE TO ADD MORE");
            break;
        }
    } while (symbol = strtok(nullptr, ","));
}

void StockWidget::setup() {
    if (m_stockCount == 0) {
        Serial.println("No stock tickers available");
        return;
    }
}

void StockWidget::draw(bool force) {
    for (int8_t i = 0; i < m_stockCount; i++) {
        if (m_stocks[i].isChanged() || force) {
            displayStock(i, m_stocks[i], TFT_WHITE, TFT_BLACK);
            m_stocks[i].setChangedStatus(false);
        }
    }
}

void StockWidget::update(bool force) {
    if (force || m_stockDelayPrev == 0 || (millis() - m_stockDelayPrev) >= m_stockDelay) {
        setBusy(true);
        for (int8_t i = 0; i < m_stockCount; i++) {
            getStockData(m_stocks[i]);
        }
        setBusy(false);
        m_stockDelayPrev = millis();
    }
}

void StockWidget::changeMode() {
    update(true);
}

void StockWidget::getStockData(StockDataModel &stock) {
    String httpRequestAddress = "https://api.twelvedata.com/quote?apikey=e03fc53524454ab8b65d91b23c669cc5&symbol=" + stock.getSymbol();

    HTTPClient http;
    http.begin(httpRequestAddress);
    int httpCode = http.GET();

    if (httpCode > 0) {  // Check for the returning code
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            float currentPrice = doc["close"].as<float>();
            if (currentPrice > 0.0) {
                stock.setCurrentPrice(doc["close"].as<float>());
                stock.setPercentChange(doc["percent_change"].as<float>()/100);
                stock.setPriceChange(doc["change"].as<float>());
                stock.setVolume(doc["acerage_volume"].as<float>());
            } else {
                Serial.println("skipping invalid data for: " + stock.getSymbol());
            }
        } else {
            // Handle JSON deserialization error
            Serial.println("deserializeJson() failed");
        }
    } else {
        // Handle HTTP request error
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void StockWidget::displayStock(int8_t displayIndex, StockDataModel &stock, uint32_t backgroundColor, uint32_t textColor) {
    Serial.println("displayStock - " + stock.getSymbol() + " ~ " + stock.getCurrentPrice());
    if (stock.getCurrentPrice() == 0.0) {
        // There isn't any data to display yet
        return;
    }
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    display.fillScreen(TFT_BLACK);
    display.setTextSize(4);  // Set text size

    // Calculate center positions
    int screenWidth = display.width();
    int centre = 120;
    int arrowOffsetX = 0;
    int arrowOffsetY = 0;


    if (stock.getPercentChange() < 0.0) {
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.fillTriangle(110 + arrowOffsetX, 120 + arrowOffsetY, 130 + arrowOffsetX, 120 + arrowOffsetY, 120 + arrowOffsetX, 132 + arrowOffsetY, TFT_RED);
    } else {
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.fillTriangle(110 + arrowOffsetX, 120 + arrowOffsetY, 130 + arrowOffsetX, 120 + arrowOffsetY, 120 + arrowOffsetX, 132 + arrowOffsetY, TFT_RED);
    }

    // Draw stock data
    display.setTextColor(TFT_BLACK);

    display.fillRect(0,68, screenWidth, 45, TFT_WHITE);  // rgb565 colors

    display.drawString(stock.getSymbol(), centre, 92, 1);


    display.drawString("$" + stock.getCurrentPrice(2), centre, 120 + display.fontHeight(1), 1);


    display.fillRect(0,0,screenWidth, 68, TFT_RED);  // rgb565 colors
    display.setTextSize(3);  // Set text size
    display.drawString(stock.getPercentChange(2) + "%", centre, 43, 1);
    display.drawArc (centre, centre, 120, 115, 0, 360, TFT_RED, TFT_RED);
  
}

String StockWidget::getName() {
    return "Stock";
}