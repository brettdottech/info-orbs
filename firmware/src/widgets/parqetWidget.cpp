#include "widgets/parqetWidget.h"

#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <HTTPClient.h>
#include <config.h>

#include <iomanip>

ParqetWidget::ParqetWidget(ScreenManager &manager) : Widget(manager) {
    Serial.println("Constructing ParqetWidget");
    ParqetDataModel parqet = ParqetDataModel();
    #ifdef PARQET_PORTFOLIO_ID
        parqet.setPortfolioId(PARQET_PORTFOLIO_ID);
    #endif
    m_portfolio = parqet;
}

void ParqetWidget::setup() {
    m_time = GlobalTime::getInstance();
    m_holdingsDisplayFrom = 0;
}


void ParqetWidget::draw(bool force) {
    // Check if we need more than one page
    bool isMultiPage = m_portfolio.getHoldingsCount() > (m_showClock ? 4 : 5);
    // Do we need to update the screens because cycle time is expired
    bool updateByCycle = isMultiPage && (millis() - m_cycleDelayPrev) >= m_cycleDelay;
    // Do we need to update the clock screen?
    bool updateByClock = m_showClock && (millis() - m_clockDelayPrev) >= m_clockDelay;
    // Do we need to update the stock screens?
    bool updateStocks = force || m_changed || updateByCycle;
    if (updateStocks || updateByClock) {
        int8_t stockDisplays = 5;
        int8_t startDisplay = 0;
        if (m_showClock) {
            // Update the clock in every update
            stockDisplays--;
            startDisplay++;
            int8_t curPage = m_holdingsDisplayFrom/stockDisplays + 1;
            int8_t totalPages = (m_portfolio.getHoldingsCount()-1) / stockDisplays + 1;
            String extra = String(curPage) + "/" + String(totalPages);
            displayClock(0, TFT_BLACK, TFT_WHITE, extra);
            m_clockDelayPrev = millis();
        }
        if (updateStocks) {
            // Update the stocks only of necessary
            for (int8_t i = 0; i < stockDisplays; i++) {
                int8_t displayIdx = startDisplay + i;
                int8_t holdingIdx = m_holdingsDisplayFrom + i;
                if (holdingIdx < m_portfolio.getHoldingsCount()) {
                    ParqetHoldingDataModel holding = m_portfolio.getHolding(holdingIdx);
                    displayStock(displayIdx, holding, TFT_WHITE, TFT_BLACK);
                } else {
                    clearScreen(displayIdx, TFT_BLACK);
                }
            }
            // In the next cycle, show the next set of stocks
            m_holdingsDisplayFrom += stockDisplays;
            if (m_holdingsDisplayFrom >= m_portfolio.getHoldingsCount()) {
                m_holdingsDisplayFrom = 0;
            }
            m_changed = false;
            m_cycleDelayPrev = millis();
        }
    }
}

void ParqetWidget::update(bool force) {
    if (force || m_stockDelayPrev == 0 || (millis() - m_stockDelayPrev) >= m_stockDelay) {
        setBusy(true);
        Serial.println("Update ParquetPortfolio");
        updatePortfolio();
        m_holdingsDisplayFrom = 0;
        m_changed = true;
        setBusy(false);
        m_stockDelayPrev = millis();
    }
}

void ParqetWidget::changeMode() {
    m_curMode++;
    if (m_curMode >= MODE_COUNT) {
        m_curMode = 0;
    }
    update(true);
}

void ParqetWidget::changeModeLongpress() {
}

String ParqetWidget::getTimeframe() {
    return m_modes[m_curMode];
}

ParqetDataModel ParqetWidget::getPortfolio() {
    return m_portfolio;
}

void ParqetWidget::updatePortfolio() {
    String portfolioId = m_portfolio.getPortfolioId();
    Serial.printf("Parqet: Update Portfolio %s\n", portfolioId.c_str());
    String httpRequestAddress = "https://api.parqet.com/v1/portfolios/assemble";
    String postPayload = "{ \"portfolioIds\": [\"" + portfolioId + "\"], \"holdingIds\": [], \"assetTypes\": [], \"timeframe\": \"" + getTimeframe() + "\"}";
    Serial.printf("POST Payload: %s\n", postPayload.c_str());
    HTTPClient http;
    const char* keys[] = {"Transfer-Encoding"};
    http.collectHeaders(keys, 1);
    http.begin(httpRequestAddress);
    http.addHeader("Content-Type", "application/json"); 

    int httpCode = http.POST(postPayload);
    Serial.printf("HTTP %d, Size %d\n", httpCode, http.getSize());

    if (httpCode == 200) {  // Check for the returning code
        Stream& rawStream = http.getStream();
        ChunkDecodingStream decodedStream(http.getStream());

        // Choose the right stream depending on the Transfer-Encoding header
        // Parqet might send chunked responses
        Stream& response =
            http.header("Transfer-Encoding") == "chunked" ? decodedStream : rawStream;

        // Parse response
        JsonDocument doc;
        JsonDocument filter;
        filter["holdings"] = true;
        filter["performance"] = true;

        // ReadLoggingStream loggingStream(response, Serial);
        // DeserializationError error = deserializeJson(doc, loggingStream, DeserializationOption::Filter(filter));
        DeserializationError error = deserializeJson(doc, response, DeserializationOption::Filter(filter));

        if (!error) {
            JsonArray holdings = doc["holdings"];
            ParqetHoldingDataModel *holdingArray = new ParqetHoldingDataModel[holdings.size()];
            int count = 0;
            for (JsonVariant holding : holdings) {
                String type = holding["assetType"].as<String>();
                if (type == "security") {
                    // stocks or etf/funds
                    String id = holding["asset"]["identifier"].as<String>();
                    String name = holding["sharedAsset"]["name"].as<String>();
                    float purchasePrice = holding["performance"]["priceAtIntervalStart"].as<float>();
                    float purchaseValue = holding["performance"]["purchaseValueForInterval"].as<float>();
                    float currentPrice = holding["position"]["currentPrice"].as<float>();
                    float currentValue = holding["position"]["currentValue"].as<float>();
                    float shares = holding["position"]["shares"].as<float>();
                    bool isSold = holding["position"]["isSold"].as<bool>();
                    String currency = holding["currency"].as<String>();
                    if (isSold || currentValue == 0) {
                        Serial.printf("Skipping %s, %s\n", name.c_str(), id.c_str());
                    } else {
                        Serial.printf("Name: %s, id: %s, cur: %s, start: %.2f, now: %.2f, delta: %.2f, deltaPct: %.2f, curValue: %.2f\n", name.c_str(), id.c_str(), currency.c_str(), purchasePrice, currentPrice, currentValue);
                        ParqetHoldingDataModel h = ParqetHoldingDataModel();
                        h.setId(id);
                        h.setName(name);
                        h.setPurchasePrice(purchasePrice);
                        h.setPurchaseValue(purchaseValue);
                        h.setCurrentPrice(currentPrice);
                        h.setCurrentValue(currentValue);
                        h.setShares(shares);
                        h.setCurrency(currency);
                        holdingArray[count++] = h;
                    }
                } else {
                    String id = holding["_id"].as<String>();
                    Serial.printf("Invalid type: %s, id: %s\n", type.c_str(), id.c_str());
                }
            }
            // Add total
            if (m_showTotal) {
                JsonVariant perf = doc["performance"];
                ParqetHoldingDataModel h = ParqetHoldingDataModel();
                h.setId("total");
                h.setName("T O T A L");
                h.setPurchasePrice(perf["purchaseValueForInterval"].as<float>());
                h.setPurchaseValue(perf["purchaseValueForInterval"].as<float>());
                h.setCurrentPrice(perf["portfolioValue"].as<float>());
                h.setCurrentValue(perf["portfolioValue"].as<float>());
                h.setShares(1);
                // AFAIK, the whole portfolio is shown in the same currency. To avoid another HTTP request, we just use the currency of the first holding
                if (count > 0) {
                    h.setCurrency(holdingArray[0].getCurrency());
                }
                holdingArray[count++] = h;
            }

            m_portfolio.setHoldings(holdingArray, count);
        } else {
            // Handle JSON deserialization error
            Serial.println("deserializeJson() failed");
            Serial.println(error.c_str());
        }
    } else {
        // Handle HTTP request error
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void ParqetWidget::clearScreen(int8_t displayIndex, int32_t background) {
    m_manager.selectScreen(displayIndex);
    TFT_eSPI &display = m_manager.getDisplay();
    display.fillScreen(background);
}

void ParqetWidget::displayClock(int8_t displayIndex, uint32_t background, uint32_t color, String extra) {
    Serial.printf("displayClock at screen %d\n", displayIndex);
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    int clky = 100;
    int centre = 120;

    display.setTextColor(color);
    display.setTextSize(1);
    display.setTextDatum(MC_DATUM);

    display.fillScreen(background);
    display.setTextColor(color);
    display.setTextSize(2);
    display.setTextDatum(MC_DATUM);
#ifdef WEATHER_UNITS_METRIC
    display.drawString(String(m_time->getDay()) + " " + m_time->getMonthName(), centre, 160, 2);
#else
    display.drawString(m_time->getMonthName() + " " + String(m_time->getDay()), centre, 160, 2);
#endif
    display.setTextColor(color);
    display.setTextDatum(MR_DATUM);
    display.setTextSize(1);

    display.drawString(m_time->getHourPadded(), centre - 5, clky, 8);

    display.setTextColor(color);
    display.setTextDatum(ML_DATUM);
    display.setTextSize(1);
    display.drawString(m_time->getMinutePadded(), centre + 5, clky, 8);
    display.setTextDatum(MC_DATUM);
    display.setTextColor(color);
    display.drawString(":", centre, clky, 8);


    display.fillRect(0, 0, 240, 50, TFT_DARKGREY);
    display.setTextDatum(MC_DATUM);
    display.setTextColor(color);
    display.setTextSize(2);
    display.drawString(extra, centre, 27, 2);

    display.fillRect(0, 190, 240, 50, TFT_DARKGREY);
    display.setTextDatum(MC_DATUM);
    display.setTextColor(color);
    display.setTextSize(2);
    display.drawString(getTimeframe(), centre, 210, 2);
}

void ParqetWidget::displayStock(int8_t displayIndex, ParqetHoldingDataModel &stock, uint32_t backgroundColor, uint32_t textColor) {
    Serial.printf("displayStock(): display=%d, stock=%s\n", displayIndex, stock.getName().c_str());
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(3);

    // Calculate center positions
    int screenWidth = display.width();
    int centre = 120;

    display.drawString(stock.getCurrency(), centre, 28, 1);
    if (m_showValues) {
        display.drawString(stock.getCurrentValue(2), centre, 62, 1);
    } else {
        display.drawString(stock.getCurrentPrice(2), centre, 62, 1);
    }

    // Draw stock data (multiline)    
    // display.fillRect(0, 80, screenWidth, 100, TFT_LIGHTGREY);
    // display.fillRect(0, 80, screenWidth, 5, TFT_DARKGREY);
    // display.fillRect(0, 175, screenWidth, 5, TFT_DARKGREY);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(2);

    String wrappedLines[MAX_WRAPPED_LINES];
    String dataValues = stock.getName();
    int yOffset = 100;
    int lineCount = Utils::getWrappedLines(wrappedLines, dataValues, 14);
    if (lineCount > MAX_STOCKNAME_LINES) {
        lineCount = MAX_STOCKNAME_LINES;
    }
    int height = 28;
    yOffset += (MAX_STOCKNAME_LINES-lineCount)*height/2;
    for (int i = 0; i < lineCount; i++) {
        display.drawString(wrappedLines[i], 120, yOffset + (height * i), 2);
    }
    display.setTextSize(3);

    if (stock.getPercentChange() < 0.0) {
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.fillRect(0, 80, screenWidth, 5, TFT_RED);
        display.fillRect(0, 175, screenWidth, 5, TFT_RED);
        // display.fillTriangle(120, 240, 130, 220, 110, 220, TFT_RED);
        display.drawArc(120, 120, 120, 115, 0, 360, TFT_RED, TFT_BLACK);
    } else {
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.fillRect(0, 80, screenWidth, 5, TFT_GREEN);
        display.fillRect(0, 175, screenWidth, 5, TFT_GREEN);
        // display.fillTriangle(120, 220, 130, 240, 110, 240, TFT_GREEN);
        display.drawArc(120, 120, 120, 115, 0, 360, TFT_GREEN, TFT_BLACK);
    }

    display.drawString(stock.getPercentChange(2) + "%", centre, 202, 1);
}