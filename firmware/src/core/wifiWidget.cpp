#include "core/wifiWidget.h"
#include <WiFi.h>
#include <utils.h>

const int lineHeight = 40;
const int statusScreenIndex = 3;

WifiWidget::WifiWidget(ScreenManager& manager) : Widget(manager) {}

WifiWidget::~WifiWidget() {}

void WifiWidget::setup() {
    TFT_eSPI &display = m_manager.getDisplay();
    display.setTextSize(1);
    display.setTextColor(TFT_WHITE);

    m_manager.selectScreen(statusScreenIndex);
    display.drawCentreString("Connecting to WiFi", ScreenCenterX, ScreenCenterY - lineHeight, 4);
    display.drawCentreString(WIFI_SSID, ScreenCenterX, ScreenCenterY, 4);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.println("Connecting to WiFi...");
}

void WifiWidget::update(bool force) {
    // Force is currently unhandled due to not knowing what behavior it would change

    if (WiFi.status() == WL_CONNECTED) {
        m_isConnected = true;
        m_connectionString = "Connected";
    } else {
        m_connectionTimer += 500;
        m_dotsString += " . ";
        Serial.print(".");
        if (m_dotsString.length() > 9) {
            m_dotsString = "";
        }
        if (m_connectionTimer > m_connectionTimeout) {
            m_connectionFailed = true;
            connectionTimedOut();
        }
    }
}

void WifiWidget::draw(bool force) {
    // Force is currently unhandled due to not knowing what behavior it would change
    TFT_eSPI &display = m_manager.getDisplay();
    m_manager.selectScreen(statusScreenIndex);
    const int blankRectTop = ScreenCenterY + lineHeight / 2;

    if (!m_isConnected && !m_connectionFailed) {
        display.fillRect(0, blankRectTop, ScreenWidth, ScreenHeight - blankRectTop, TFT_BLACK);
        display.drawCentreString(m_dotsString, ScreenCenterX, ScreenCenterY + lineHeight, 4);
    } else if (m_isConnected && !m_hasDisplayedSuccess) {
        m_hasDisplayedSuccess = true;
        display.fillScreen(TFT_BLACK);
        display.drawCentreString("Success", ScreenCenterX, ScreenCenterY, 4);
        Serial.println();
        Serial.println("Connected to WiFi");
        m_isConnected = true;
    } else if (m_connectionFailed && !m_hasDisplayedError) {
        m_hasDisplayedError = true;
        display.fillRect(0, blankRectTop, ScreenWidth, ScreenHeight - blankRectTop, TFT_BLACK);
        display.drawCentreString(m_connectionString, ScreenCenterX, ScreenCenterY + lineHeight, 4);
    }
}

void WifiWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
}

void WifiWidget::connectionTimedOut() {
  switch (WiFi.status()) {
  case WL_CONNECTED:
    m_connectionString = "Connected";
    break;
  case WL_NO_SSID_AVAIL:
    m_connectionString = "No SSID available";
    break;
  case WL_CONNECT_FAILED:
    m_connectionString = "Connection failed";
    break;
  case WL_IDLE_STATUS:
    m_connectionString = "Idle status";
    break;
  case WL_DISCONNECTED:
    m_connectionString = "Disconnected";
    break;
  default:
    m_connectionString = "Unknown";
    break;
  }
}

String WifiWidget::getName() {
    return "WiFi";
}