#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "SPIFFS.h"
#include "screenManager.h"

class WifiManager {
   public:
    WifiManager();
    void setupWifiManagement();
    bool isConnected();
    void setup(ScreenManager& manager);
    void draw(ScreenManager& manager);

   private:
    AsyncWebServer m_server{80};

    IPAddress m_localIP;
    IPAddress m_localGateway;
    IPAddress m_subnet{255, 255, 0, 0};

    // Search for parameter in HTTP POST request
    const char* PARAM_INPUT_1 = "ssid";
    const char* PARAM_INPUT_2 = "pass";
    const char* PARAM_INPUT_3 = "ip";
    const char* PARAM_INPUT_4 = "gateway";

    // Variables to save values from HTML form
    String m_ssid;
    String m_pass;
    String m_ip;
    String m_gateway;

    String m_dotsString{"."};

    // File paths to save input values permanently
    const char* m_ssidPath = "/ssid.txt";
    const char* m_passPath = "/pass.txt";
    const char* m_ipPath = "/ip.txt";
    const char* m_gatewayPath = "/gateway.txt";

    unsigned long m_previousMillis = 0;
    const long m_interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

    void initSPIFFS();
    String readFile(fs::FS& fs, const char* path);
    void writeFile(fs::FS& fs, const char* path, const char* message);
    // String processor(const String& var);
    bool initialize();
};