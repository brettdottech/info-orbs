
#include "manager.h"

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "SPIFFS.h"
#include "screenManager.h"

WifiManager::WifiManager() {}

// Initialize SPIFFS
void WifiManager::initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    Serial.println("SPIFFS mounted successfully");
}

void WifiManager::setup(ScreenManager& manager) {
    TFT_eSPI& display = manager.getDisplay();
    manager.selectAllScreens();
    display.fillScreen(TFT_BLACK);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE);

    manager.selectScreen(0);
    display.drawCentreString("Connect to the", 120, 80, 1);

    manager.selectScreen(1);
    display.fillScreen(TFT_BLACK);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE);
    display.drawCentreString("InfoOrbs-WiFi-Manager", 0, 80, 1);
}

// Read File from SPIFFS
String WifiManager::readFile(fs::FS& fs, const char* path) {
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return String();
    }

    String fileContent;
    while (file.available()) {
        fileContent = file.readStringUntil('\n');
        break;
    }
    return fileContent;
}

// Write file to SPIFFS
void WifiManager::writeFile(fs::FS& fs, const char* path, const char* message) {
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
}

// Initialize WiFi
bool WifiManager::initialize() {
    if (m_ssid == "" || m_ip == "") {
        Serial.println("Undefined SSID or IP address.");
        return false;
    }

    WiFi.mode(WIFI_STA);
    m_localIP.fromString(m_ip.c_str());
    m_localGateway.fromString(m_gateway.c_str());

    if (!WiFi.config(m_localIP, m_localGateway, m_subnet)) {
        Serial.println("STA Failed to configure");
        return false;
    }
    WiFi.begin(m_ssid.c_str(), m_pass.c_str());
    Serial.println("Connecting to WiFi...");

    unsigned long currentMillis = millis();
    m_previousMillis = currentMillis;

    while (WiFi.status() != WL_CONNECTED) {
        currentMillis = millis();
        if (currentMillis - m_previousMillis >= m_interval) {
            Serial.println("Failed to connect.");
            return false;
        }
    }

    Serial.println(WiFi.localIP());
    return true;
}

// Replaces placeholder with LED state value
String processor(const String& var) {
    return String("");
}

bool WifiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WifiManager::setupWifiManagement() {
    // Load values saved in SPIFFS
    m_ssid = readFile(SPIFFS, m_ssidPath);
    m_pass = readFile(SPIFFS, m_passPath);
    m_ip = readFile(SPIFFS, m_ipPath);
    m_gateway = readFile(SPIFFS, m_gatewayPath);
    Serial.println(m_ssid);
    Serial.println(m_pass);
    Serial.println(m_ip);
    Serial.println(m_gateway);

    if (this->initialize()) {
        // Route for root / web page
        m_server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/index.html", "text/html", false, processor);
        });
        m_server.serveStatic("/", SPIFFS, "/");

        // Route to set GPIO state to HIGH
        m_server.on("/on", HTTP_GET, [this](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/index.html", "text/html", false, processor);
        });

        // Route to set GPIO state to LOW
        m_server.on("/off", HTTP_GET, [this](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/index.html", "text/html", false, processor);
        });
        m_server.begin();
    } else {
        // Connect to Wi-Fi network with SSID and password
        Serial.println("Setting AP (Access Point)");
        // NULL sets an open Access Point
        WiFi.softAP("InfoOrbs-WiFi-Manager", NULL);

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        // Web Server Root URL
        m_server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/wifimanager.html", "text/html");
        });

        m_server.serveStatic("/", SPIFFS, "/");

        m_server.on("/", HTTP_POST, [this](AsyncWebServerRequest* request) {
            int params = request->params();
            for (int i = 0; i < params; i++) {
                AsyncWebParameter* p = request->getParam(i);
                if (p->isPost()) {
                    // HTTP POST m_ssid value
                    if (p->name() == PARAM_INPUT_1) {
                        m_ssid = p->value().c_str();
                        Serial.print("SSID set to: ");
                        Serial.println(m_ssid);
                        // Write file to save value
                        writeFile(SPIFFS, m_ssidPath, m_ssid.c_str());
                    }
                    // HTTP POST pass value
                    if (p->name() == PARAM_INPUT_2) {
                        m_pass = p->value().c_str();
                        Serial.print("Password set to: ");
                        Serial.println(m_pass);
                        // Write file to save value
                        writeFile(SPIFFS, m_passPath, m_pass.c_str());
                    }
                    // HTTP POST ip value
                    if (p->name() == PARAM_INPUT_3) {
                        m_ip = p->value().c_str();
                        Serial.print("IP Address set to: ");
                        Serial.println(m_ip);
                        // Write file to save value
                        writeFile(SPIFFS, m_ipPath, m_ip.c_str());
                    }
                    // HTTP POST gateway value
                    if (p->name() == PARAM_INPUT_4) {
                        m_gateway = p->value().c_str();
                        Serial.print("Gateway set to: ");
                        Serial.println(m_gateway);
                        // Write file to save value
                        writeFile(SPIFFS, m_gatewayPath, m_gateway.c_str());
                    }
                    // Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
            }
            request->send(200, "text/plain", "Done. InfoOrbs will restart, connect to your router and go to IP address: " + m_ip);
            delay(3000);
            ESP.restart();
        });
        m_server.begin();
    }
};