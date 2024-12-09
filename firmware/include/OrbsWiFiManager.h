#include <WiFiManager.h>

class OrbsWiFiManager : public WiFiManager {
public:
    // Constructor
    OrbsWiFiManager() : WiFiManager() {
        _showBack = true; // Set _showBack to true
    }
};