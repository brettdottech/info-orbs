#ifndef WIDGET_H
#define WIDGET_H

#include <screenManager.h>
#include <config.h>

const uint8_t LEFT_BUTTON_PRESSED = 0;
const uint8_t LEFT_BUTTON_PRESSED_LONG = 1;
const uint8_t MIDDLE_BUTTON_PRESSED = 2;
const uint8_t MIDDLE_BUTTON_PRESSED_LONG = 3;
const uint8_t RIGHT_BUTTON_PRESSED = 4;
const uint8_t RIGHT_BUTTON_PRESSED_LONG = 5;


class Widget {
public:
    Widget(ScreenManager& manager);
    virtual ~Widget() = default;
    virtual void setup() = 0;
    virtual void update(bool force = false) = 0;
    virtual void draw(bool force = false) = 0;
    virtual void buttonPressed(uint8_t buttonId) = 0;
    virtual String getName() = 0;
    void setBusy(bool busy);

protected:
    ScreenManager& m_manager;
};
#endif // WIDGET_H