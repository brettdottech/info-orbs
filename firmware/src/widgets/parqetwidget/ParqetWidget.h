#ifndef PARQET_WIDGET_H
#define PARQET_WIDGET_H

#include "GlobalTime.h"
#include "ParqetDataModel.h"
#include "ShowMemoryUsage.h"
#include "Utils.h"
#include "Widget.h"
#include "config_helper.h"

#ifdef PARQET_DEBUG
    #define PARQET_DEBUG_PRINT(...) PARQET_DEBUG_PRINT_IMPL(__VA_ARGS__, false)
    #define PARQET_DEBUG_PRINT_IMPL(msg, ...)                     \
        do {                                                      \
            char buffer[128];                                     \
            snprintf(buffer, sizeof(buffer), msg, ##__VA_ARGS__); \
            if (buffer[0] != '\0') {                              \
                Serial.println(buffer);                           \
            }                                                     \
        } while (0)
    #define PARQET_DEBUG_PRINT_MEM(...) PARQET_DEBUG_PRINT_MEM_IMPL(__VA_ARGS__, false)
    #define PARQET_DEBUG_PRINT_MEM_IMPL(msg, ...)                 \
        do {                                                      \
            char buffer[128];                                     \
            snprintf(buffer, sizeof(buffer), msg, ##__VA_ARGS__); \
            SHOW_MEMORY_USAGE(buffer);                            \
        } while (0)
#else
    #define PARQET_DEBUG_PRINT(msg, ...) // No-op
    #define PARQET_DEBUG_PRINT_MEM(msg, ...) // No-op
#endif

#define PARQET_MODE_COUNT 10
#define PARQET_PERF_COUNT 6
#define PARQET_PERF_CHART_COUNT 5
#define PARQET_MAX_STOCKNAME_LINES 3

class ParqetWidget : public Widget {
public:
    ParqetWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    String getTimeframe();
    void updatePortfolio();
    void processResponse(int httpCode, const String &response);
    void updatePortfolioChart();
    void processResponseChart(int httpCode, const String &response);
    void displayStock(int8_t displayIndex, ParqetHoldingDataModel &stock, uint32_t backgroundColor, uint32_t textColor);
    ParqetDataModel getPortfolio();
    void clearScreen(int8_t displayIndex, int32_t background);
    void displayClock(int8_t displayIndex, uint32_t background, uint32_t color, String extra, uint32_t extraColor);

    GlobalTime *m_time;

    unsigned long m_stockDelay = 15 * 60 * 1000; // default to 15m between updates
    unsigned long m_stockDelayPrev = 0;

    unsigned long m_cycleDelay = 30 * 1000; // cycle through pages (for more than 4/5 stocks) every 30 seconds
    unsigned long m_cycleDelayPrev = 0;

    unsigned long m_clockDelay = 60 * 1000; // update the clock every minute
    unsigned long m_clockDelayPrev = 0;

    String m_modes[PARQET_MODE_COUNT] = {"today", "1w", "1m", "3m", "6m", "1y", "3y", "mtd", "ytd", "max"}; // Possible timeframes: today, 1w, 1m, 3m, 6m, 1y, 3y, mtd, ytd, max
    int m_defaultMode = 0;
    int m_curMode = 0;

    String m_perfMeasures[PARQET_PERF_COUNT] = {"totalReturnGross", "totalReturnNet", "returnGross", "returnNet", "ttwror", "izf"};
    int m_defaultPerfMeasure = 0;
    int m_curPerfMeasure = 0;

    String m_perfChartMeasures[PARQET_PERF_CHART_COUNT] = {"perfHistory", "perfHistoryUnrealized", "ttwror", "drawdown", "none"};
    int m_defaultPerfChartMeasure = 0;
    int m_curPerfChartMeasure = 0;

    boolean m_showClock = true; // Show clock on first screen
    boolean m_showTotalScreen = true; // Show a total portfolio screen
    boolean m_showTotalValue = false; // Show your total portfolio value
    boolean m_showTotalChart = true; // Show performance chart for total (if we have more than 7 datapoints, ie. not for "today")
    String m_overrideTotalChartToday = "1w"; // Show this chart for "today" to have a chart there as well, set to empty string to disable
    int m_showValues = 0; // Show current price (0) or value in portfolio (1)

#ifdef PARQET_PORTFOLIO_ID
    std::string m_portfolioId = PARQET_PORTFOLIO_ID;
#else
    std::string m_portfolioId = "";
#endif
    std::string m_parquetProxyUrl = "https://parqet-proxy.ce-data.net/proxy";
    ParqetDataModel m_portfolio;
    int m_holdingsDisplayFrom = 0;
    boolean m_changed = false;
    boolean m_everDrawn = false; // Track if our widget was ever drawn (to distinguish between an onboot and an onwidget update)
};
#endif // PARQET_WIDGET_H
