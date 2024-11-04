#ifndef WIDGET_H
#define WIDGET_H

#include <screenManager.h>
#include <config_helper.h>
#include <Button.h>


class Widget {
public:
    Widget(ScreenManager& manager);
    virtual ~Widget() = default;
    virtual void setup() = 0;
    virtual void update(bool force = false) = 0;
    virtual void draw(bool force = false) = 0;
    virtual void buttonPressed(uint8_t buttonId, ButtonState state) = 0;
    virtual String getName() = 0;
    void setBusy(bool busy);

protected:
    ScreenManager& m_manager;
};
#endif // WIDGET_H