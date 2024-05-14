#include <widgetSet.h>

WidgetSet::WidgetSet(ScreenManager *sm) {
  screenManager = sm;
}
void WidgetSet::add(Widget *widget) {
  if (_widgetCount == MAX_WIDGETS) {
    Serial.println("MAX WIDGETS UNABLE TO ADD");
    return;
  }
  _widgets[_widgetCount] = widget;
  _widgets[_widgetCount]->setup();
  _widgetCount++;

}

void WidgetSet::drawCurrent() {
  _widgets[_currentWidget]->draw();
}
void WidgetSet::updateCurrent() {
  _widgets[_currentWidget]->update();
}

Widget *WidgetSet::getCurrent() {
  return _widgets[_currentWidget];
}

void WidgetSet::next() {
  _currentWidget++;
  if (_currentWidget >= _widgetCount) {
    _currentWidget = 0;
  }
  _switchWidget();
}

void WidgetSet::prev() {
  _currentWidget--;
  if (_currentWidget < 0) {
    _currentWidget = _widgetCount-1;
  }
  _switchWidget();
}

void WidgetSet::_switchWidget() {
  screenManager->clearAllScreens();
  getCurrent()->setup();
}
