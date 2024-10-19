#include "core/wifiWidget.h"
#include <WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager


WifiWidget::WifiWidget(ScreenManager& manager) : Widget(manager) {}

WifiWidget::~WifiWidget() {}

// WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifimgr;

void WifiWidget::setup() {
  TFT_eSPI &display = m_manager.getDisplay();
  m_manager.selectAllScreens();
  display.fillScreen(TFT_BLACK);
  display.setTextSize(2);
  display.setTextColor(TFT_WHITE);

  m_manager.selectScreen(0);
  display.drawCentreString("Connecting" + m_connectionString, 120, 80, 1);

  WiFi.mode(WIFI_STA); // For WiFiManager explicitly set mode to station, esp defaults to STA+AP

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp WiFi library
  // wifimgr.resetSettings();

  // WiFiManager automatically connects using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  // Set WiFiManager to non-blocking so status and info can be displayed
  wifimgr.setConfigPortalBlocking(false);
  // wm.setConfigPortalTimeout(60);

  // wm.autoConnect(); // auto generated AP name from chipid
  // wm.autoConnect("AutoConnectAP"); // anonymous AP
  // wifimgr.autoConnect("AutoConnectAP","password"); // password protected AP
  if(wifimgr.autoConnect("Info-Orbs")){
    Serial.print("WifiManager connected.");
  }
  else {
    m_configPortalRunning = true;
    Serial.println("Configuration portal running.");
    m_manager.selectScreen(1);
    display.drawCentreString("Configure WiFi", 120, 80, 1);
    display.drawCentreString("by browsing to:", 120, 100, 1);
    display.drawCentreString("192.168.4.1", 120, 120, 1);
  }
}

void WifiWidget::update(bool force) {
  //force is currently an unhandled due to not knowing what behavior it would change

  // If WiFiManager is non-blocking, this keeps the configuration portal running
  wifimgr.process();

	if(WiFi.status() == WL_CONNECTED) {
		m_isConnected = true;
		m_connectionString = "Connected";
    m_ipaddress = WiFi.localIP().toString();
    Serial.print("IP address: ");
    Serial.println(m_ipaddress);

	} else {
		m_connectionTimer += 500;
		m_dotsString += ".";
    Serial.print(".");
		if(m_dotsString.length() > 3) {
			m_dotsString = "";
		}
		if(m_connectionTimer > m_connectionTimeout && !m_configPortalRunning) {
			m_connectionFailed = true;
			connectionTimedOut();
		}
	}
}

void WifiWidget::draw(bool force) {
  //force is currently an unhandled due to not knowing what behavior it would change

	if(!m_isConnected && !m_connectionFailed) {
		TFT_eSPI &display = m_manager.getDisplay();
		m_manager.selectScreen(0);
		display.fillRect(0, 100, 240, 100, TFT_BLACK);
		display.drawCentreString(m_dotsString, 120, 100, 1);
	} else if(m_isConnected && !m_hasDisplayedSuccess) {
		m_hasDisplayedSuccess = true;
		TFT_eSPI &display = m_manager.getDisplay();
		m_manager.selectScreen(0);
		display.fillScreen(TFT_BLACK);
		display.drawCentreString("Connected", 120, 100, 1);
    m_manager.selectScreen(1);
    display.drawCentreString("IP Address:", 120, 80, 1);
    display.drawCentreString(m_ipaddress, 120, 120, 1);

    Serial.println();
    Serial.println("Connected to WiFi");
		m_isConnected = true;
	} else if(m_connectionFailed && !m_hasDisplayedError) {
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
}
