#include <WiFiManager.h>

/**
 * We override WiFiManager in order to enable "_showBack" (show back buttons on WebPortal pages).
 * Currently, there is no setter for this in WiFiManager and overriding makes updating
 * the library easier than including the library source
 */
class OrbsWiFiManager : public WiFiManager {
public:
    OrbsWiFiManager() {
        _showBack = true; // Set _showBack to true
    }
};