#include "ParqetWidget.h"

#include "ParqetTranslations.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <StreamUtils.h>
#include <TaskFactory.h>
#include <iomanip>

ParqetWidget::ParqetWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config) {
    Serial.printf("Constructing ParqetWidget, portfolioId=%s\n", m_portfolioId.c_str());
    m_config.addConfigBool("ParqetWidget", "pqEnabled", &m_enabled, i18n(t_enableWidget));
    m_config.addConfigString("ParqetWidget", "pqportfoId", &m_portfolioId, 50, i18n(t_cnfPortfolioId));
    m_config.addConfigComboBox("ParqetWidget", "pqDefMode", &m_defaultMode, m_modes, PARQET_MODE_COUNT, i18n(t_cnfMode), true);
    m_config.addConfigComboBox("ParqetWidget", "pqDefPerf", &m_defaultPerfMeasure, m_perfMeasures, PARQET_PERF_COUNT, i18n(t_cnfPerfMeasure), true);
    m_config.addConfigComboBox("ParqetWidget", "pqDefPerfCh", &m_defaultPerfChartMeasure, m_perfChartMeasures, PARQET_PERF_CHART_COUNT, i18n(t_cnfChartMeasure), true);
    m_config.addConfigBool("ParqetWidget", "pqShowClock", &m_showClock, i18n(t_cnfClock), true);
    m_config.addConfigBool("ParqetWidget", "pqShowTotalScr", &m_showTotalScreen, i18n(t_cnfTotals), true);
    m_config.addConfigBool("ParqetWidget", "pqShowTotalVal", &m_showTotalValue, i18n(t_cnfTotalVal), true);
    String optPriceVal[] = {"Show current price", "Show current value"};
    m_config.addConfigComboBox("ParqetWidget", "pqShowValues", &m_showValues, optPriceVal, 2, i18n(t_cnfValues), true);
    m_config.addConfigString("ParqetWidget", "pqProxyUrl", &m_proxyUrl, 75, i18n(t_cnfProxyUrl), true);
    m_curMode = m_defaultMode;
    m_curPerfMeasure = m_defaultPerfMeasure;
    m_curPerfChartMeasure = m_defaultPerfChartMeasure;
}

void ParqetWidget::setup() {
    m_time = GlobalTime::getInstance();
    m_holdingsDisplayFrom = 0;
}

void ParqetWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);
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
            int8_t curPage = m_holdingsDisplayFrom / stockDisplays + 1;
            int8_t totalPages = (m_portfolio.getHoldingsCount() - 1) / stockDisplays + 1;
            String extra = String(curPage) + "/" + String(totalPages);
            displayClock(0, TFT_BLACK, TFT_WHITE, extra, TFT_DARKGREY);
            m_clockDelayPrev = millis();
        }
        if (updateStocks) {
            // Update the stocks only if necessary
            for (int8_t i = 0; i < stockDisplays; i++) {
                int8_t displayIdx = startDisplay + i;
                int8_t holdingIdx = m_holdingsDisplayFrom + i;
                if (holdingIdx < m_portfolio.getHoldingsCount()) {
                    ParqetHoldingDataModel holding = m_portfolio.getHolding(holdingIdx);
                    displayStock(displayIdx, holding, TFT_BLACK, TFT_WHITE);
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
    m_everDrawn = true;
}

void ParqetWidget::update(bool force) {
    if (force || m_stockDelayPrev == 0 || (millis() - m_stockDelayPrev) >= m_stockDelay) {
        Serial.println("Update ParqetPortfolio");
        if (m_everDrawn && m_showClock) {
            displayClock(0, TFT_BLACK, TFT_WHITE, "Updating", TFT_RED);
        }
        updatePortfolio();
        m_stockDelayPrev = millis();
    }
}

void ParqetWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        // Force drawing to show the next set of stocks
        draw(true);
    } else if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        // Change timeframe and force update
        m_curMode++;
        if (m_curMode >= PARQET_MODE_COUNT) {
            m_curMode = 0;
        }
        update(true);
    } else if (buttonId == BUTTON_OK && state == BTN_LONG) {
        // Reset view to default timeframe and reload
        m_curMode = m_defaultMode;
        update(true);
    }
}

String ParqetWidget::getTimeframe() {
    if (m_curMode < PARQET_MODE_COUNT) {
        return m_modes[m_curMode];
    } else {
        return m_modes[0];
    }
}

String ParqetWidget::getPerfMeasure() {
    if (m_curPerfMeasure < PARQET_PERF_COUNT) {
        return m_perfMeasures[m_curPerfMeasure];
    } else {
        return m_perfMeasures[0];
    }
}

String ParqetWidget::getPerfChartMeasure() {
    if (m_curPerfChartMeasure < PARQET_PERF_CHART_COUNT) {
        return m_perfChartMeasures[m_curPerfChartMeasure];
    } else {
        return m_perfChartMeasures[0];
    }
}

ParqetDataModel ParqetWidget::getPortfolio() {
    return m_portfolio;
}

void ParqetWidget::updatePortfolio() {
    PARQET_DEBUG_PRINT_MEM("Begin .updatePortfolio()");
    if (m_portfolioId.empty() || m_proxyUrl.empty()) {
        return;
    }
    Serial.printf("Parqet: Update Portfolio %s\n", m_portfolioId.c_str());
    String httpRequestAddress = String(m_proxyUrl.c_str());
    httpRequestAddress += "?id=" + String(m_portfolioId.c_str()) + "&timeframe=" + getTimeframe() + "&perf=" + getPerfMeasure() + "&perfChart=" + getPerfChartMeasure();

    auto task = TaskFactory::createHttpGetTask(
        httpRequestAddress, [this](int httpCode, const String &response) {
            processResponse(httpCode, response);
        });

    if (!task) {
        Serial.println("Failed to create parqet task");
        return;
    }

    bool success = TaskManager::getInstance()->addTask(std::move(task));
    if (!success) {
        Serial.println("Failed to add parqet task");
        return;
    }
}

void ParqetWidget::processResponse(int httpCode, const String &response) {
    PARQET_DEBUG_PRINT_MEM("start processResponse()");
    PARQET_DEBUG_PRINT("HTTP %d, Size %d", httpCode, response.length());

    // Check for the returning code
    if (httpCode == 200) {
        // Parse response
        JsonDocument doc;

        PARQET_DEBUG_PRINT_MEM("Parsing portfolio JSON now...");
        DeserializationError error = deserializeJson(doc, response);
        PARQET_DEBUG_PRINT_MEM("after deserializeJson()");

        if (!error) {
            JsonArray holdings = doc["holdings"];
            // Initialize a new array (reserver one extra element for totals)
            auto *holdingArray = new ParqetHoldingDataModel[holdings.size() + 1];
            PARQET_DEBUG_PRINT_MEM("after new holdingArray");
            int count = 0;
            for (JsonVariant holding : holdings) {
                String type = holding["assetType"].as<String>();
                String id = holding["id"].as<String>();
                String name = holding["name"].as<String>();
                float purchasePrice = holding["priceStart"].as<float>();
                float purchaseValue = holding["valueStart"].as<float>();
                float currentPrice = holding["priceNow"].as<float>();
                float currentValue = holding["valueNow"].as<float>();
                float shares = holding["shares"].as<float>();
                float perf = holding["perf"].as<float>();
                String currency = holding["currency"].as<String>();
                if (currentValue > 0) {
                    ParqetHoldingDataModel h = ParqetHoldingDataModel();
                    h.setId(id);
                    h.setName(name);
                    h.setPurchasePrice(purchasePrice);
                    h.setPurchaseValue(purchaseValue);
                    h.setCurrentPrice(currentPrice);
                    h.setCurrentValue(currentValue);
                    h.setShares(shares);
                    h.setCurrency(currency);
                    h.setPerformance(perf);
                    holdingArray[count++] = h;
                }
            }
            // Add total
            if (m_showTotalScreen) {
                JsonVariant perf = doc["performance"];
                ParqetHoldingDataModel h = ParqetHoldingDataModel();
                h.setId("total");
                h.setName("T O T A L");
                h.setPurchasePrice(perf["valueStart"].as<float>());
                h.setPurchaseValue(perf["valueStart"].as<float>());
                h.setCurrentPrice(perf["valueNow"].as<float>());
                h.setCurrentValue(perf["valueNow"].as<float>());
                h.setPerformance(perf["perf"].as<float>());
                h.setShares(1);
                // AFAIK, the whole portfolio is shown in the same currency.
                // To avoid another HTTP request, we just use the currency of the first holding
                if (count > 0) {
                    h.setCurrency(holdingArray[0].getCurrency());
                }
                holdingArray[count++] = h;
            }
            PARQET_DEBUG_PRINT_MEM("pre setHoldings()");
            m_portfolio.setHoldings(holdingArray, count);
            PARQET_DEBUG_PRINT_MEM("after setHoldings()");
            JsonArray chart = doc["chart"];
            float *chartsArray = new float[chart.size()];
            count = 0;
            for (JsonVariant val : chart) {
                chartsArray[count++] = val.as<float>();
            }
            m_portfolio.setChartData(chartsArray, count);

        } else {
            // Handle JSON deserialization error
            Serial.println("deserializeJson() failed");
            Serial.println(error.c_str());
        }
    } else {
        // Handle HTTP request error
        Serial.printf("HTTP request failed, error: %d\n", httpCode);
    }

    PARQET_DEBUG_PRINT_MEM("Parqet portfolio update complete");
    m_holdingsDisplayFrom = 0;
    m_changed = true;
}

void ParqetWidget::clearScreen(int8_t displayIndex, int32_t background) {
    m_manager.selectScreen(displayIndex);
    m_manager.fillScreen(background);
}

void ParqetWidget::displayClock(int8_t displayIndex, uint32_t background, uint32_t color, String extra, uint32_t extraColor) {
    // Serial.printf("displayClock at screen %d\n", displayIndex);
    m_manager.selectScreen(displayIndex);

    int clky = 105;

    m_manager.fillScreen(background);
    m_manager.setFontColor(color);
    m_manager.drawString(m_time->getDayAndMonth(), ScreenCenterX, clky + 60, 18, Align::MiddleCenter);

    m_manager.drawString(m_time->getHourPadded(), ScreenCenterX - 10, clky, 66, Align::MiddleRight);
    m_manager.drawString(m_time->getMinutePadded(), ScreenCenterX + 10, clky, 66, Align::MiddleLeft);
    m_manager.drawString(":", ScreenCenterX, clky, 66, Align::MiddleCenter);

    m_manager.fillRect(0, 0, 240, 50, extraColor);
    m_manager.setBackgroundColor(extraColor);
    m_manager.drawString(extra, ScreenCenterX, 27, 18, Align::MiddleCenter);

    m_manager.fillRect(0, 190, 240, 50, extraColor);
    m_manager.drawString(getTimeframe(), ScreenCenterX, 212, 22, Align::MiddleCenter);
}

void ParqetWidget::displayStock(int8_t displayIndex, ParqetHoldingDataModel &stock, uint32_t backgroundColor, uint32_t textColor) {
    PARQET_DEBUG_PRINT("displayStock(): display=%d, stock=%s", displayIndex, stock.getName().c_str());
    m_manager.selectScreen(displayIndex);
    m_manager.fillScreen(backgroundColor);
    m_manager.setFontColor(textColor);

    m_manager.drawString(stock.getCurrency(), ScreenCenterX, 27, 15, Align::MiddleCenter);
    if (stock.getId() != "total" || m_showTotalValue) {
        if (m_showValues) {
            m_manager.drawString(stock.getCurrentValue(2), ScreenCenterX, 58, 26, Align::MiddleCenter);
        } else {
            m_manager.drawString(stock.getCurrentPrice(2), ScreenCenterX, 58, 26, Align::MiddleCenter);
        }
    } else {
        m_manager.drawString(i18n(t_portfolio), ScreenCenterX, 58, 26, Align::MiddleCenter);
    }

    if (m_showTotalChart && stock.getId() == "total" && m_portfolio.getChartDataCount() >= 7) {
        // total with chart (we only plot this when we have at least 7 data points)
        int chartDataCount = m_portfolio.getChartDataCount();
        float *chartData = m_portfolio.getChartData();
        float scale, minVal, maxVal, chartMinVal;
        int chartHeight = 83;
        int maxChartData = 200;
        int endLine = 171;
        int spaceInBetween = (maxChartData / chartDataCount) - 1;
        int xOffset = (240 - (spaceInBetween + 1) * (chartDataCount - 1)) / 2;
        m_portfolio.getChartDataScale(chartHeight, scale, minVal, maxVal, chartMinVal);
        int zeroAtY = endLine + round(chartMinVal * scale);
        // Serial.printf("Scale: %f, minVal: %f, maxVal: %f, zeroAtY: %d, siB=%d, xOff=%d\n", scale, minVal, maxVal, zeroAtY, spaceInBetween, xOffset);
        for (int i = 0; i < chartDataCount; i++) {
            int x = (spaceInBetween + 1) * i + xOffset;
            int y = zeroAtY - round(chartData[i] * scale);
            bool positive = chartData[i] >= 0;
            // Serial.printf("Drawing line %d, v=%f, @ %d/%d\n", i, chartData[i], x, y);
            if (spaceInBetween == 0) {
                // Draw one line
                m_manager.drawLine(x, zeroAtY, x, y, positive ? TFT_DARKGREEN : TFT_RED);
            } else if (spaceInBetween == 1) {
                // Draw two lines
                m_manager.drawLine(x, zeroAtY, x, y, positive ? TFT_DARKGREEN : TFT_RED);
                m_manager.drawLine(x + 1, zeroAtY, x + 1, y, positive ? TFT_DARKGREEN : TFT_RED);
            } else {
                // Draw rect
                int myY = zeroAtY;
                // Calc height
                int h = y - zeroAtY;
                if (h < 0) {
                    // Height is negative, but fillRect() does not support this
                    // Move Y and invert height
                    h *= -1;
                    myY -= h;
                }
                // Serial.printf("Drawing rect %d, v=%f, @ %d/%d/%d/%d\n", i, chartData[i], x - spaceInBetween/2, myY, spaceInBetween, h);
                m_manager.fillRect(x - spaceInBetween / 2, myY, spaceInBetween, h, positive ? TFT_DARKGREEN : TFT_RED);
            }
        }
        // display.drawLine(0, zeroAtY, 240, zeroAtY, TFT_WHITE);
        m_manager.fillRect(0, zeroAtY - 1, 240, 3, TFT_WHITE);
        int minAtY = zeroAtY - round(minVal * scale);
        int maxAtY = zeroAtY - round(maxVal * scale);
        // Serial.printf("min/max lines would be at %d/%d\n", minAtY, maxAtY);
        if (zeroAtY < minAtY - 15 || zeroAtY > minAtY) {
            // Show minVal if the zero line is not interfering
            m_manager.drawLine(0, minAtY, 240, minAtY, TFT_DARKGREY);
            m_manager.drawString(String(minVal) + "%", 25, minAtY, 11, Align::BottomLeft);
        }
        if (zeroAtY > maxAtY + 15 || zeroAtY < maxAtY) {
            // Show maxVal if the zero line is not interfering
            m_manager.drawLine(0, maxAtY, 240, maxAtY, TFT_DARKGREY);
            m_manager.drawString(String(maxVal) + "%", 25, maxAtY, 11, Align::TopLeft);
        }
    } else {
        // Draw stock data (multiline)
        String wrappedLines[MAX_WRAPPED_LINES];
        String dataValues = stock.getName();
        int yOffset = 100;
        int lineCount = Utils::getWrappedLines(wrappedLines, dataValues, 14);
        if (lineCount > PARQET_MAX_STOCKNAME_LINES) {
            lineCount = PARQET_MAX_STOCKNAME_LINES;
        }
        int height = 30;
        yOffset += (PARQET_MAX_STOCKNAME_LINES - lineCount) * height / 2;
        for (int i = 0; i < lineCount; i++) {
            // Lager font if we need less lines
            int fontSize = 17 + 6 / lineCount;
            m_manager.drawString(wrappedLines[i], 120, yOffset + (height * i), fontSize, Align::MiddleCenter);
        }
    }

    uint32_t stockColor = TFT_DARKGREY;
    if (stock.getPerformance() < 0) {
        stockColor = TFT_RED;
    } else if (stock.getPerformance() > 0) {
        stockColor = TFT_DARKGREEN;
    }

    m_manager.setFontColor(stockColor);
    m_manager.fillRect(0, 80, ScreenWidth, 5, stockColor);
    m_manager.fillRect(0, 176, ScreenWidth, 5, stockColor);
    m_manager.drawArc(120, 120, 120, 115, 0, 360, stockColor, backgroundColor);
    m_manager.drawString(stock.getPerformance(2) + "%", ScreenCenterX, 205, 22, Align::MiddleCenter);
}

String ParqetWidget::getName() {
    return "Parqet";
}
