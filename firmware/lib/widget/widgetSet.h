#ifndef WIDGET_SET_H
#define WIDGET_SET_H

#include <widget.h>
#include <screenManager.h>
#include <utils.h>

#define MAX_WIDGETS 5

class WidgetSet {
public:
    WidgetSet(ScreenManager *sm);
    void add(Widget *widget);
    void drawCurrent(bool force=false);
    void updateCurrent();
    Widget *getCurrent();
    void next();
    void prev();
    void buttonPressed(uint8_t buttonId, ButtonState state);
    void showLoading();
    void updateAll();
    bool initialUpdateDone();
    void initializeAllWidgetsData();
    void setClearScreensOnDrawCurrent();
    void updateBrightnessByTime(uint8_t hour24);

   private:
    void showCenteredLine(int screen, const String& text);
    ScreenManager *m_screenManager;
    bool m_clearScreensOnDrawCurrent = true;
    Widget *m_widgets[MAX_WIDGETS];
    int8_t m_widgetCount = 0;
    int8_t m_currentWidget = 0;

    bool m_initialized = false;

    void switchWidget();
};
#endif // WIDGET_SET_H