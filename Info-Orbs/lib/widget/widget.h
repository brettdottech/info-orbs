#include <screenManager.h>

class Widget {
public:
    Widget(ScreenManager& manager);
    virtual ~Widget() = default;
    virtual void draw() = 0;
    virtual void update() = 0;

protected:
    ScreenManager& m_manager;
};