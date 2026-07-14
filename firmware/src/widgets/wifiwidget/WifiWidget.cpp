#include "WifiWidget.h"
#include "OrbsWiFiManager.h"
#include "Utils.h"
#include <ArduinoLog.h>
#include <ESPmDNS.h>
#include <WiFi.h>

const int lineHeight = 40;
const int statusScreenIndex = 3;
const int fontSize = 19;
const int messageDelay = 5000;

// Ascending TX power tiers. We start low (to reduce the current spike from
// the radio while the displays are already drawing steady current) and only
// escalate if the lower tiers fail to connect.
const wifi_power_t txPowerTiers[] = {
    WIFI_POWER_8_5dBm,
    WIFI_POWER_11dBm,
    WIFI_POWER_13dBm,
    WIFI_POWER_15dBm,
    WIFI_POWER_17dBm,
    WIFI_POWER_19_5dBm,
};
const int numTxPowerTiers = sizeof(txPowerTiers) / sizeof(txPowerTiers[0]);
const unsigned long txPowerAttemptTimeout = 5000; // ms to wait for a connection at each power tier

WifiWidget::WifiWidget(ScreenManager &manager, ConfigManager &config, WiFiManager &wifiManager) : Widget(manager, config), m_wifiManager(wifiManager) {}

WifiWidget::~WifiWidget() {}

void WifiWidget::setup() {
    m_manager.setFont(DEFAULT_FONT);
    m_manager.selectScreen(statusScreenIndex);
    m_manager.clearScreen();
    m_manager.setFontColor(TFT_WHITE);
    m_manager.drawCentreString("Connecting", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);

    // Hold right button when connecting to power to reset wifi settings
    // these are stored by the ESP WiFi library
    if (digitalRead(BUTTON_RIGHT_PIN) == Button::PRESSED_LEVEL) {
        m_wifiManager.resetSettings();
        m_manager.drawCentreString("Wifi Settings reset", ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        delay(messageDelay);
    }

    WiFi.mode(WIFI_STA); // For WiFiManager explicitly set mode to station, ESP defaults to STA+AP

#if (defined WIFI_SSID && defined WIFI_PASS)
    // Preload credentials from config.h
    m_wifiManager.preloadWiFi(WIFI_SSID, WIFI_PASS);
#endif

    // Remove unwanted buttons from the config portal
    std::vector<const char *> wm_menu = {"wifi", "param", "custom", "info", "restart"}; // buttons: wifi, info, exit, update
    // Remove unwanted buttons from the Info page
    m_wifiManager.setShowInfoUpdate(false);
    m_wifiManager.setShowInfoErase(false);
    // Add buttons link
    const char *customMenuHtml = "<form action='/browse' method='get'><button>Browse Filesystem</button></form><br/>\n"
                                 "<form action='/buttons' method='get'><button>Buttons</button></form><br/>\n";
    m_wifiManager.setCustomMenuHTML(customMenuHtml);
    m_wifiManager.setMenu(wm_menu);
    m_wifiManager.setClass("invert"); // Dark mode
    m_wifiManager.setShowStaticFields(true);

    // Set WiFiManager to non-blocking so status and info can be displayed
    m_wifiManager.setConfigPortalBlocking(false);

    // If you want the config portal to only be available for so many seconds
    // wm.setConfigPortalTimeout(60);

    // Use a static SSID to be able to show a static QR Code
    m_apssid = "InfoOrbs";

    m_wifiManager.setCleanConnect(true);
    m_wifiManager.setConnectRetries(5);

    // Set hostname to InfoOrbs-<last 6 digits of MAC address> so it can be found on the network
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    String hostname = "InfoOrbs-" + mac.substring(mac.length() - 6);
    m_wifiManager.setHostname(hostname);

    Log.noticeln("Hostname: %s", hostname.c_str());

    // Try connecting at a reduced TX power first, escalating only if needed.
    // This avoids stacking a full-power radio TX burst on top of the displays'
    // already-steady current draw, which can brown out a marginal power supply.
    String savedSsid = m_wifiManager.getWiFiSSID();
    String savedPass = m_wifiManager.getWiFiPass();
    if (savedSsid.length() > 0) {
        bool preConnected = tryConnectWithEscalation(savedSsid, savedPass);
        Serial.printf("Pre-connect at reduced TX power %s.\n", preConnected ? "succeeded" : "did not succeed, falling back to WiFiManager");
    }

    // WiFiManager automatically connects using saved credentials...
    // (if preConnected is true, this returns almost immediately since WiFi is already connected)
    if (m_wifiManager.autoConnect(m_apssid.c_str())) {
        Log.infoln("WifiManager connected.");
    } else { // ...if connection fails (no saved credentials), it starts an access point with a WiFi setup portal at 192.168.4.1
        m_configPortalRunning = true;
        Log.infoln("Configuration portal running.");
        m_manager.selectScreen(statusScreenIndex);
        m_manager.clearScreen();
        m_manager.drawCentreString("Configure", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.selectScreen(statusScreenIndex + 1);
        m_manager.drawCentreString("Connect", ScreenCenterX, ScreenCenterY - lineHeight * 2, fontSize);
        m_manager.drawCentreString("phone or PC", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.drawCentreString("to WiFi network:", ScreenCenterX, ScreenCenterY, fontSize);
        m_manager.setFontColor(TFT_SKYBLUE);
        m_manager.drawCentreString(m_apssid, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        m_manager.setFontColor(TFT_GREENYELLOW);
        m_manager.drawCentreString("192.168.4.1", ScreenCenterX, ScreenCenterY + lineHeight * 2, fontSize);
    }
}

void WifiWidget::update(bool force) {
    // Force is currently unhandled due to not knowing what behavior it would change

    // If WiFiManager is non-blocking, this keeps the configuration portal running
    m_wifiManager.process();

    if (WiFi.status() == WL_CONNECTED) {
        m_isConnected = true;
        m_connectionString = "Connected";
        m_ipaddress = WiFi.localIP().toString();
        Log.infoln("IP address: %s", m_ipaddress.c_str());
        // Start the WebPortal
        m_wifiManager.startWebPortal();
#ifdef INCLUDE_MDNS
        // Initialize mDNS
        String mDNSname = m_wifiManager.getWiFiHostname();
        if (!MDNS.begin(mDNSname)) {
            Log.warningln("Error setting up MDNS responder!");
        } else {
            Log.infoln("mDNS responder started. You should find this device at http://%s\n", mDNSname.c_str());
        }
        MDNS.addService("http", "tcp", 80);
#endif
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
    m_manager.selectScreen(statusScreenIndex);
    const int blankRectTop = ScreenCenterY + lineHeight / 2;

    if (!m_isConnected && !m_connectionFailed) {
        m_manager.fillRect(0, blankRectTop, ScreenWidth, ScreenHeight - blankRectTop, TFT_BLACK);
        m_manager.drawCentreString(m_dotsString, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
    } else if (m_isConnected && !m_hasDisplayedSuccess) {
        m_hasDisplayedSuccess = true;
        m_manager.clearScreen();
        m_manager.drawCentreString("Success", ScreenCenterX, ScreenCenterY, fontSize);
        m_manager.selectScreen(statusScreenIndex + 1);
        m_manager.clearScreen();
        m_manager.drawCentreString("IP Address", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.drawCentreString(m_ipaddress, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        Log.infoln("Connected to WiFi");
        delay(messageDelay);
    } else if (m_connectionFailed && !m_hasDisplayedError) {
        m_hasDisplayedError = true;
        m_manager.fillRect(0, blankRectTop, ScreenWidth, ScreenHeight - blankRectTop, TFT_BLACK);
        m_manager.drawCentreString(m_connectionString, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        delay(messageDelay);
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

// Passively scan (no probe requests, so negligible TX current) for the saved
// SSID's RSSI and use it to guess the lowest TX power tier likely to reach it.
// Signal strength on the AP->client link is a reasonable proxy for the
// power needed on the client->AP link, though not a guaranteed match.
int WifiWidget::selectStartingTxPowerIndex(const String &ssid) {
    int startIndex = 1; // default: skip only the very lowest tier if we can't measure RSSI
    int numNetworks = WiFi.scanNetworks(false /*async*/, false /*show_hidden*/, true /*passive*/, 110 /*ms per channel*/);
    for (int i = 0; i < numNetworks; i++) {
        if (WiFi.SSID(i) == ssid) {
            int rssi = WiFi.RSSI(i);
            Serial.printf("Found saved SSID '%s' at RSSI %d during passive scan.\n", ssid.c_str(), rssi);
            if (rssi > -50) {
                startIndex = 0; // very strong signal, try the lowest power tier
            } else if (rssi > -65) {
                startIndex = 1;
            } else if (rssi > -75) {
                startIndex = 3;
            } else {
                startIndex = numTxPowerTiers - 1; // weak signal, go straight to full power
            }
            break;
        }
    }
    WiFi.scanDelete();
    return startIndex;
}

// Attempts to connect starting at a reduced TX power, escalating one tier at a
// time on failure. Returns true if connected, leaving WiFi connected; returns
// false if every tier was exhausted, leaving WiFi disconnected for the caller
// (WiFiManager::autoConnect) to take over.
bool WifiWidget::tryConnectWithEscalation(const String &ssid, const String &pass) {
    int startIndex = selectStartingTxPowerIndex(ssid);
    for (int i = startIndex; i < numTxPowerTiers; i++) {
        Serial.printf("Attempting WiFi connect at TX power tier %d.\n", (int) txPowerTiers[i]);
        WiFi.setTxPower(txPowerTiers[i]);
        WiFi.begin(ssid.c_str(), pass.c_str());

        unsigned long attemptStart = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - attemptStart < txPowerAttemptTimeout) {
            delay(100);
        }

        if (WiFi.status() == WL_CONNECTED) {
            return true;
        }
        WiFi.disconnect(true);
    }
    return false;
}

String WifiWidget::getName() {
    return "WiFi";
}
