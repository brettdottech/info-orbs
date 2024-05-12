#include "widgets/screenWidgets/stockScreenWidget.h"

StockScreenWidget::StockScreenWidget(ScreenManager &sm,const String& stock, int screenIndex): ScreenWidget(sm, screenIndex), m_TrackingStock(stock) {
    httpRequestAddress = "https://finnhub.io/api/v1/quote?symbol=" + m_TrackingStock + "&token=c1tjb52ad3ia4h4uee9g";
}

void StockScreenWidget::setup() {}

void StockScreenWidget::draw() {
    m_ScreenManager.selectScreen(m_ScreenIndex);
    Serial.println("Drawing stock screen" + String(m_ScreenIndex));
    TFT_eSPI display = m_ScreenManager.getDisplay();
    m_ScreenManager.reset();
}

void StockScreenWidget::update() {
}
