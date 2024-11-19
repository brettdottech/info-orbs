#include "core/wifiWidget.h"
#include <WiFi.h>
#include <utils.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

const int lineHeight = 40;
const int statusScreenIndex = 3;

WifiWidget::WifiWidget(ScreenManager& manager) : Widget(manager) {}

WifiWidget::~WifiWidget() {}

WiFiManager wifimgr;

void WifiWidget::setup() {
    TFT_eSPI &display = m_manager.getDisplay();
    display.setTextSize(1);
    display.setTextColor(TFT_WHITE);

    m_manager.selectScreen(statusScreenIndex);
    display.drawCentreString("Connecting", ScreenCenterX, ScreenCenterY - lineHeight, 4);

    WiFi.mode(WIFI_STA); // For WiFiManager explicitly set mode to station, ESP defaults to STA+AP

#if(defined WIFI_SSID && defined WIFI_PASS)
    m_hardCodedWiFi = true;
    WiFi.begin(WIFI_SSID, WIFI_PASS);
#endif

    // Remove unwanted buttons from the config portal
    std::vector<const char *> wm_menu  = {"wifi"};  // buttons: wifi, info, exit, update
    // Remove unwanted buttons from the Info page
    wifimgr.setShowInfoUpdate(false);
    wifimgr.setShowInfoErase(false);
    wifimgr.setMenu(wm_menu);
  
    // reset settings - wipe stored credentials for testing
    // these are stored by the ESP WiFi library
    //wifimgr.resetSettings();

    // Set WiFiManager to non-blocking so status and info can be displayed
    wifimgr.setConfigPortalBlocking(false);

    // If you want the config portal to only be available for so many seconds
    // wm.setConfigPortalTimeout(60);

    // Add the last 2 digits of the MAC address onto the end of the config portal SSID
    // so each Info-Orbs has a unique SSID
		m_apssid = "Info-Orbs_" + WiFi.macAddress().substring(15);

    // WiFiManager automatically connects using saved credentials...
    if (wifimgr.autoConnect(m_apssid.c_str())) {
      Serial.print("WifiManager connected.");
    } else {  // ...if connection fails (no saved credentials), it starts an access point with a WiFi setup portal at 192.168.4.1
      m_configPortalRunning = true;
      Serial.println("Configuration portal running.");
      m_manager.selectScreen(statusScreenIndex);
      display.fillScreen(TFT_BLACK);
      display.drawCentreString("Configure", ScreenCenterX, ScreenCenterY - lineHeight, 4);
      m_manager.selectScreen(statusScreenIndex + 1);
      display.drawCentreString("Connect", ScreenCenterX, ScreenCenterY - lineHeight * 2, 4);
      display.drawCentreString("phone or PC", ScreenCenterX, ScreenCenterY - lineHeight, 4);
      display.drawCentreString("to WiFi network:", ScreenCenterX, ScreenCenterY, 4);
      display.setTextColor(TFT_SKYBLUE);
      display.drawCentreString(m_apssid, ScreenCenterX, ScreenCenterY + lineHeight, 4);
      display.setTextColor(TFT_GREENYELLOW);
      display.drawCentreString("192.168.4.1", ScreenCenterX, ScreenCenterY + lineHeight * 2, 4);
    }
}

void WifiWidget::update(bool force) {
    // Force is currently unhandled due to not knowing what behavior it would change

    // If WiFiManager is non-blocking, this keeps the configuration portal running
    wifimgr.process();

    if (WiFi.status() == WL_CONNECTED) {
        m_isConnected = true;
        m_connectionString = "Connected";
        m_ipaddress = WiFi.localIP().toString();
        Serial.print("IP address: ");
        Serial.println(m_ipaddress);
    } else {
        m_connectionTimer += 500;
        m_dotsString += " . ";
        Serial.print(".");
        if (m_dotsString.length() > 9) {
            m_dotsString = "";
        }
		if (m_connectionTimer > m_connectionTimeout && !m_configPortalRunning) {
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
        m_manager.selectScreen(statusScreenIndex + 1);
        display.drawCentreString("IP Address", ScreenCenterX, ScreenCenterY - lineHeight, 4);
        display.drawCentreString(m_ipaddress, ScreenCenterX, ScreenCenterY + lineHeight, 4);
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