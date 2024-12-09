#ifndef WIDGET_H
#define WIDGET_H

#include "Button.h"
#include "ConfigManager.h"
#include "ScreenManager.h"
#include "config_helper.h"

class Widget {
public:
    Widget(ScreenManager &manager, ConfigManager &config);
    virtual ~Widget() = default;
    virtual void setup() = 0;
    virtual void update(bool force = false) = 0;
    virtual void draw(bool force = false) = 0;
    virtual void buttonPressed(uint8_t buttonId, ButtonState state) = 0;
    virtual String getName() = 0;
    void setBusy(bool busy);
    bool isEnabled();

protected:
    ScreenManager &m_manager;
    ConfigManager &m_config;
    bool m_enabled = false;
};
#endif // WIDGET_H