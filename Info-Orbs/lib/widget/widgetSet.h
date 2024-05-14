#ifndef WIDGET_SET_H
#define WIDGET_SET_H

#include <widget.h>
#include <screenManager.h>

#define MAX_WIDGETS 5

class WidgetSet {
public:
    WidgetSet(ScreenManager *sm);
    void add(Widget *widget);
    void drawCurrent();
    void updateCurrent();
    Widget *getCurrent();
    void next();
    void prev();

private:
    ScreenManager *screenManager;
    Widget *_widgets[MAX_WIDGETS];
    int8_t _widgetCount = 0;
    int8_t _currentWidget = 0;

    void _switchWidget();
};
#endif // WIDGET_SET_H