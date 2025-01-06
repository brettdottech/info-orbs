#include "StockWidget.h"
#include "TaskFactory.h"
#include <ArduinoJson.h>
#include <iomanip>

StockWidget::StockWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config) {
    m_enabled = true; // Enabled by default
    m_config.addConfigBool("StockWidget", "stocksEnabled", &m_enabled, "Enable Widget");
    config.addConfigString("StockWidget", "stockList", &m_stockList, 200,
                           "Choose 5 securities to track. You can track forex, crypto (symbol/USD) or stocks from any exchange (if one ticker is part of multiple exchanges you can add on '&country = Canada' to narrow down to your ticker)");
    char stockList[m_stockList.size()];
    strcpy(stockList, m_stockList.c_str());

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
    m_manager.setFont(DEFAULT_FONT);
    for (int8_t i = 0; i < m_stockCount; i++) {
        if (m_stocks[i].isChanged() || force) {
            displayStock(i, m_stocks[i], TFT_WHITE, TFT_BLACK);
            m_stocks[i].setChangedStatus(false);
        }
    }
}

void StockWidget::update(bool force) {
    if (force || m_stockDelayPrev == 0 || (millis() - m_stockDelayPrev) >= m_stockDelay) {
        
        // Queue requests for each stock
        for (int8_t i = 0; i < m_stockCount; i++) {
            String url = "https://api.twelvedata.com/quote?apikey=e03fc53524454ab8b65d91b23c669cc5&symbol=" + m_stocks[i].getSymbol();
            
            StockDataModel& stock = m_stocks[i];

            auto task = TaskFactory::createHttpTask(url, [this, &stock](int httpCode, const String &response) {
                processResponse(stock, httpCode, response);
            });

            TaskManager::getInstance()->addTask(std::move(task));
        }

        m_stockDelayPrev = millis();
    }
}

void StockWidget::processResponse(StockDataModel& stock, int httpCode, const String& response) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            float currentPrice = doc["close"].as<float>();
            if (currentPrice > 0.0) {
                stock.setCurrentPrice(doc["close"].as<float>());
                stock.setPercentChange(doc["percent_change"].as<float>() / 100);
                stock.setPriceChange(doc["change"].as<float>());
                stock.setHighPrice(doc["fifty_two_week"]["high"].as<float>());
                stock.setLowPrice(doc["fifty_two_week"]["low"].as<float>());
                stock.setCompany(doc["name"].as<String>());
                stock.setTicker(doc["symbol"].as<String>());
                stock.setCurrencySymbol(doc["currency"].as<String>());
            } else {
                Serial.println("skipping invalid data for: " + stock.getSymbol());
            }
        } else {
            Serial.println("deserializeJson() failed");
        }
    } else {
        Serial.printf("HTTP request failed, error: %d\n", httpCode);
    }
}

void StockWidget::changeMode() {
    update(true);
}

void StockWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        changeMode();
}

void StockWidget::displayStock(int8_t displayIndex, StockDataModel &stock, uint32_t backgroundColor, uint32_t textColor) {
    Serial.println("displayStock - " + stock.getSymbol() + " ~ " + stock.getCurrentPrice());
    if (stock.getCurrentPrice() == 0.0) {
        // There isn't any data to display yet
        return;
    }
    m_manager.selectScreen(displayIndex);

    m_manager.fillScreen(TFT_BLACK);

    // Calculate center positions
    int screenWidth = SCREEN_SIZE;
    int centre = 120;
    int arrowOffsetX = 0;
    int arrowOffsetY = -109;

    // Outputs
    m_manager.fillRect(0, 70, screenWidth, 49, TFT_WHITE);
    m_manager.fillRect(0, 111, screenWidth, 20, TFT_LIGHTGREY);
    int smallFontSize = 11;
    int bigFontSize = 29;
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString("52 Week:", centre, 185, smallFontSize);
    m_manager.drawCentreString("H: " + stock.getCurrencySymbol() + stock.getHighPrice(), centre, 200, smallFontSize);
    m_manager.drawCentreString("L: " + stock.getCurrencySymbol() + stock.getLowPrice(), centre, 215, smallFontSize);
    m_manager.setFontColor(TFT_BLACK, TFT_LIGHTGREY);
    m_manager.drawString(stock.getCompany(), centre, 121, smallFontSize, Align::MiddleCenter);
    if (stock.getPercentChange() < 0.0) {
        m_manager.setFontColor(TFT_RED, TFT_BLACK);
        m_manager.fillTriangle(110 + arrowOffsetX, 120 + arrowOffsetY, 130 + arrowOffsetX, 120 + arrowOffsetY, 120 + arrowOffsetX, 132 + arrowOffsetY, TFT_RED);
        m_manager.drawArc(centre, centre, 120, 118, 0, 360, TFT_RED, TFT_RED);
    } else {
        m_manager.setFontColor(TFT_GREEN, TFT_BLACK);
        m_manager.fillTriangle(110 + arrowOffsetX, 132 + arrowOffsetY, 130 + arrowOffsetX, 132 + arrowOffsetY, 120 + arrowOffsetX, 120 + arrowOffsetY, TFT_GREEN);
        m_manager.drawArc(centre, centre, 120, 118, 0, 360, TFT_GREEN, TFT_GREEN);
    }
    m_manager.drawString(stock.getPercentChange(2) + "%", centre, 48, bigFontSize, Align::MiddleCenter);
    // Draw stock data
    m_manager.setFontColor(TFT_BLACK, TFT_WHITE);

    m_manager.drawString(stock.getTicker(), centre, 92, bigFontSize, Align::MiddleCenter);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    m_manager.drawString(stock.getCurrencySymbol() + stock.getCurrentPrice(2), centre, 155, bigFontSize, Align::MiddleCenter);
}

String StockWidget::getName() {
    return "Stock";
}
