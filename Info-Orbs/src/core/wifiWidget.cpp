#include "core/wifiWidget.h"

#include <WiFi.h>

WifiWidget::WifiWidget(ScreenManager &manager) : Widget(manager) {}

WifiWidget::~WifiWidget() {}

void WifiWidget::setup() {
    TFT_eSPI &display = m_manager.getDisplay();
    m_manager.selectAllScreens();
    display.fillScreen(TFT_BLACK);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE);

    m_manager.selectScreen(0);
    display.drawCentreString("Connecting" + m_connectionString, 120, 80, 1);

    m_manager.selectScreen(1);
    display.drawCentreString("Connecting to", 120, 80, 1);
    display.drawCentreString("WiFi..", 120, 100, 1);
    display.drawCentreString(WIFI_SSID, 120, 130, 1);

    // Serial.println("Connecting to WiFi..");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.println("Connecting to WiFi..");
}

void WifiWidget::update(bool force) {
    // force is currently an unhandled due to not knowing what behavior it would change

    if (WiFi.status() == WL_CONNECTED) {
        m_isConnected = true;
        m_connectionString = "Connected";
    } else {
        m_connectionTimer += 500;
        m_dotsString += ".";
        Serial.print(".");
        if (m_dotsString.length() > 3) {
            m_dotsString = "";
        }
        if (m_connectionTimer > m_connectionTimeout) {
            m_connectionFailed = true;
            connectionTimedOut();
        }
    }
}

void WifiWidget::draw(bool force) {
    // force is currently an unhandled due to not knowing what behavior it would change

    if (!m_isConnected && !m_connectionFailed) {
        TFT_eSPI &display = m_manager.getDisplay();
        m_manager.selectScreen(0);
        display.fillRect(0, 100, 240, 100, TFT_BLACK);
        display.drawCentreString(m_dotsString, 120, 100, 1);
    } else if (m_isConnected && !m_hasDisplayedSuccess) {
        m_hasDisplayedSuccess = true;
        TFT_eSPI &display = m_manager.getDisplay();
        m_manager.selectScreen(0);
        display.fillScreen(TFT_BLACK);
        display.drawCentreString("Connected", 120, 100, 1);
        Serial.println();
        Serial.println("Connected to WiFi");
        m_isConnected = true;
    } else if (m_connectionFailed && !m_hasDisplayedError) {
        m_hasDisplayedError = true;
        TFT_eSPI &display = m_manager.getDisplay();
        m_manager.selectScreen(0);
        display.drawCentreString("Connection", 120, 80, 1);
        display.fillRect(0, 100, 240, 100, TFT_BLACK);
        display.drawCentreString(m_connectionString, 120, 100, 1);
    }
}

void WifiWidget::changeMode() {}

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

    // Serial.println("Connection timed out");
    // TFT_eSPI &display = m_manager.getDisplay();
    // m_manager.selectScreen(0);
    // display.drawCentreString("Connection", 120, 80, 1);
    // display.drawCentreString(m_connectionString, 120, 100, 1);
}
