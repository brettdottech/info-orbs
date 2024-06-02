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
    void changeMode();
    void showLoading();
    void updateAll();
    bool initialUpdateDone();
    void initializeAllWidgets();
    void shouldInitScreens(bool init);

   private:
    ScreenManager *m_screenManager;
    bool m_shouldInitScreens = true;
    Widget *m_widgets[MAX_WIDGETS];
    int8_t m_widgetCount = 0;
    int8_t m_currentWidget = 0;

    bool m_initialized = false;

    void switchWidget();
};
#endif // WIDGET_SET_H