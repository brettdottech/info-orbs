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
    void buttonPressed(uint8_t buttonId) override;
    String getName() override;

    bool isConnected() { return m_isConnected; }

private:

    void connectionTimedOut();

    bool m_isConnected{ false };
    bool m_connectionFailed{ false };

    bool m_hasDisplayedError{ false };
    bool m_hasDisplayedSuccess{ false };

    
    String m_connectionString{ "" };
    String m_dotsString{ "" };
    int m_connectionTimer{ 0 };
    const int m_connectionTimeout{ 10000 };

};

#endif // WIFIWIDGET_H