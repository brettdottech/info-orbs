#pragma once
#ifndef WIFIWIDGET_H
#define WIFIWIDGET_H

#include <widget.h>

class WifiWidget : public Widget{
public:
    WifiWidget(ScreenManager& manager);
    ~WifiWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

    bool isConnected() { return m_isConnected; }

private:

    void connectionTimedOut();

    bool m_isConnected{ false };
    bool m_connectionFailed{ false };
    bool m_hardCodedWiFi{false};
    bool m_hasDisplayedError{ false };
    bool m_hasDisplayedSuccess{ false };
    bool m_configPortalRunning{ false }; 
    
    String m_connectionString{ "" };
    String m_dotsString{ "" };
    String m_ipaddress{ "" };
    String m_apssid{ "" };
    int m_connectionTimer{ 0 };
    const int m_connectionTimeout{ 10000 };

};

#endif // WIFIWIDGET_H