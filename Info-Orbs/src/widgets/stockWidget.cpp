#include "widgets/stockWidget.h"
#include <config.h>
#include <sstream>
#include <iomanip>
#include <HTTPClient.h>
#include <ArduinoJson.h>

StockWidget::StockWidget(ScreenManager &manager) : Widget(manager), currentStockIndex(0), m_stockDelay(6000) {
}

void StockWidget::setup() {
    m_stockDelayPrev = 0;
    currentStockIndex = 0;
    stockTickers = STOCK_TICKERS;
}

void StockWidget::draw(bool force) {
    // Get initial stock then every m_stockDelay milliseconds
    if (m_stockDelayPrev == 0 || (millis() - m_stockDelayPrev) >= m_stockDelay) {
        getStockData();
        displayStock(currentStockIndex, TFT_WHITE, TFT_BLACK);
        m_stockDelayPrev = millis();
        currentStockIndex = (currentStockIndex + 1) % stockTickers.size();
    }
}

void StockWidget::update(bool force) {
}

void StockWidget::changeMode() {}

void StockWidget::getStockData() {
    if (stockTickers.size() == 0) {
        Serial.println("No stock tickers available");
        return;
    }

    m_trackingStock = stockTickers[currentStockIndex];

    String httpRequestAddress = "https://finnhub.io/api/v1/quote?symbol=" + m_trackingStock + "&token=c1tjb52ad3ia4h4uee9g";

    HTTPClient http;
    http.begin(httpRequestAddress);
    int httpCode = http.GET();
    
    if (httpCode > 0) { // Check for the returning code
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
            // Convert currentPrice to float and format to two decimal places
            float currentPriceFloat = doc["c"].as<float>();
            std::ostringstream currentPriceStream;
            currentPriceStream << std::fixed << std::setprecision(2) << currentPriceFloat;
            m_currentPrice = String(currentPriceStream.str().c_str());
            
            // Convert percentChange to float and format to two decimal places
            float percentChangeFloat = doc["dp"].as<float>();
            std::ostringstream percentChangeStream;
            percentChangeStream << std::fixed << std::setprecision(2) << percentChangeFloat;
            m_percentChange = String(percentChangeStream.str().c_str());
            
            // Convert other fields to String
            m_change = doc["d"].as<String>();
            m_highPrice = doc["h"].as<String>();
            m_lowPrice = doc["l"].as<String>();
            m_openPrice = doc["o"].as<String>();
            m_previousClosePrice = doc["pc"].as<String>();
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

void StockWidget::displayStock(int displayIndex, uint32_t backgroundColor, uint32_t textColor){
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(4); // Set text size

    // Calculate center positions
    int screenWidth = display.width();
    int centre = 120;
    
    // Draw stock data
    display.fillRect(0, 0, screenWidth, 50, 0x0256); // rgb565 colors
    display.drawString(m_trackingStock, centre, 27, 1);
    display.drawString("$" + m_currentPrice, centre, 51 + display.fontHeight(1), 1);

    if (m_percentChange.substring(0, 1) == "-") {
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.fillTriangle(120, 220, 140, 185, 100, 185, TFT_RED);
    } else {
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.fillTriangle(120, 185, 140, 220, 100, 220, TFT_GREEN);
    }

    display.drawString(m_percentChange + "%", centre, 147, 1);
}