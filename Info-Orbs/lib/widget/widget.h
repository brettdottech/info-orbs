#include <screenManager.h>

class Widget {
public:
    Widget(const ScreenManager& manager);
    virtual ~Widget() = default;
    virtual void draw() = 0;
    virtual void update() = 0;

private:
    const ScreenManager& manager;
};