#include <widgetSet.h>

WidgetSet::WidgetSet(ScreenManager *sm) : m_screenManager(sm) {

}
void WidgetSet::add(Widget *widget) {
  if (m_widgetCount == MAX_WIDGETS) {
    Serial.println("MAX WIDGETS UNABLE TO ADD");
    return;
  }
  m_widgets[m_widgetCount] = widget;
  m_widgets[m_widgetCount]->setup();
  m_widgetCount++;

}

void WidgetSet::drawCurrent() {
  m_widgets[m_currentWidget]->draw();
}
void WidgetSet::updateCurrent() {
  m_widgets[m_currentWidget]->update();
}

Widget *WidgetSet::getCurrent() {
  return m_widgets[m_currentWidget];
}

void WidgetSet::next() {
  m_currentWidget++;
  if (m_currentWidget >= m_widgetCount) {
    m_currentWidget = 0;
  }
  switchWidget();
}

void WidgetSet::prev() {
  m_currentWidget--;
  if (m_currentWidget < 0) {
    m_currentWidget = m_widgetCount-1;
  }
  switchWidget();
}

void WidgetSet::switchWidget() {
  m_screenManager->clearAllScreens();
  getCurrent()->setup();
  getCurrent()->draw(true);
}
