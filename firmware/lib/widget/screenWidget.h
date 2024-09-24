#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <screenManager.h>

class ScreenWidget {
public: 
    ScreenWidget(ScreenManager& manager, int screenIndex);
    virtual void setup() = 0;
    virtual void draw() = 0;
    virtual void update() = 0;

protected:
    int m_ScreenIndex;
    ScreenManager& m_ScreenManager;
};

#endif // SCREENWIDGET_H