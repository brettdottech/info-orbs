#include "StockWidget.h"

#include "config_helper.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <iomanip>

StockWidget::StockWidget(ScreenManager &manager) : Widget(manager), m_taskHandle(NULL) {
#ifdef STOCK_TICKER_LIST
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
#endif
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
        if (m_taskHandle == NULL) { // Check if task is not already running
            setBusy(true);
            // Create a task to handle all stock updates
            if (xTaskCreate(taskGetStockData, "StockDataTask", 8192, this, 1, &m_taskHandle) == pdPASS) {
                Serial.println("StockDataTask created");
                m_stockDelayPrev = millis();
            } else {
                Serial.println("Failed to create StockDataTask");
                setBusy(false);
            }
        } else {
            // Serial.println("StockDataTask is already running");
        }
    }
}

void StockWidget::changeMode() {
    update(true);
}

void StockWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        changeMode();
}

void StockWidget::getStockData(void *pvParameters, String stockSymbols) {
    String httpRequestAddress = "https://api.twelvedata.com/quote?apikey=e03fc53524454ab8b65d91b23c669cc5&symbol=" + stockSymbols;

    HTTPClient http;
    http.begin(httpRequestAddress);
    int httpCode = http.GET();

    StockWidget *widget = static_cast<StockWidget*>(pvParameters);

    if (httpCode > 0) { // Check for the returning code
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            JsonObject obj = doc.as<JsonObject>();
            int index = 0; 
            for (JsonPair keyValue : doc.as<JsonObject>()) {
                const char *key = keyValue.key().c_str();
                JsonObject value = keyValue.value().as<JsonObject>();
                StockDataModel stock;
                float currentPrice = value["close"].as<float>();
                if (currentPrice > 0.0) {
                   widget->m_stocks[index].setCurrentPrice(value["close"].as<float>()); 
                   widget->m_stocks[index].setPercentChange(value["percent_change"].as<float>() / 100);
                   widget->m_stocks[index].setPriceChange(value["change"].as<float>());
                   widget->m_stocks[index].setHighPrice(value["fifty_two_week"]["high"].as<float>());
                   widget->m_stocks[index].setLowPrice(value["fifty_two_week"]["low"].as<float>());
                   widget->m_stocks[index].setCompany(value["name"].as<String>());
                   widget->m_stocks[index].setTicker(value["symbol"].as<String>());
                   widget->m_stocks[index].setCurrencySymbol(value["currency"].as<String>());                    
                } else {
                   Serial.println("skipping invalid data for: " + value["symbol"].as<String>()); 
                }
                index++;
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

void StockWidget::taskGetStockData(void *pvParameters) {
    StockWidget *widget = static_cast<StockWidget*>(pvParameters);

    String stockSymbols;
    for (int8_t i = 0; i < widget->m_stockCount; i++) {
        if (!stockSymbols.isEmpty()) {
            stockSymbols += ",";
        }
        stockSymbols += widget->m_stocks[i].getSymbol();
    }
    widget->getStockData(pvParameters, stockSymbols);
    
    // The following code is useful for tuning the space allocated for this task. 
    // The highWater variable represents the free space remaing for this task (in words) 
    // UBaseType_t highWater = uxTaskGetStackHighWaterMark(NULL);
    // Serial.print("Stock Widget: Remaining task stack space: ");
    // Serial.println(highWater);
    
    widget->setBusy(false);
    widget->m_taskHandle = NULL;     
    vTaskDelete(NULL); 
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
