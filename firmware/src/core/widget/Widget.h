#ifndef WIDGET_H
#define WIDGET_H

#include "Button.h"
#include "ConfigManager.h"
#include "ScreenManager.h"
#include "Translations.h" // include for use by all Widgets
#include "config_helper.h"

class WidgetTimer;

class Widget {
public:
    Widget(ScreenManager &manager, ConfigManager &config);
    virtual ~Widget() = default;
    virtual void setup() = 0;
    virtual void update(bool force = false) = 0;
    virtual void draw(bool force = false) = 0;
    virtual void buttonPressed(uint8_t buttonId, ButtonState state) = 0;
    virtual String getName() = 0;
    // Non-zero overrides global widget cycle delay while this widget is active.
    virtual unsigned long getWidgetCyclePageDelayMs() const { return 0; }

    WidgetTimer &addDrawRefreshFrequency(TimeFrequency frequency);
    WidgetTimer &addUpdateRefreshFrequency(TimeFrequency frequency);
    void resetTimer(WidgetTimer &timer);

    void setBusy(bool busy);
    bool isEnabled();

    bool isItTimeToDraw();
    bool isItTimeToUpdate();

    // New methods to get last trigger times
    unsigned long getLastDrawTime() const; // Time of last draw trigger
    unsigned long getLastUpdateTime() const; // Time of last update trigger

protected:
    ScreenManager &m_manager;
    ConfigManager &m_config;
    bool m_enabled = false;

    WidgetTimer *m_drawTimer = nullptr;
    WidgetTimer *m_updateTimer = nullptr;
};

class WidgetTimer {
public:
    WidgetTimer(uint32_t interval) : m_interval(interval), m_previousMillis(0) {}

    bool isDue() {
        unsigned long currentMillis = millis();
        if (currentMillis - m_previousMillis >= m_interval) {
            m_previousMillis = currentMillis;
            return true;
        }
        return false;
    }

    void reset() {
        m_previousMillis = millis();
    }

    uint32_t getInterval() const { return m_interval; }
    void setInterval(uint32_t interval) { m_interval = interval; }

private:
    uint32_t m_interval;
    unsigned long m_previousMillis;
};

#endif // WIDGET_H
