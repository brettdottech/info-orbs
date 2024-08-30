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
  if (m_clearScreensOnDrawCurrent) {
    m_screenManager->clearAllScreens();
    m_clearScreensOnDrawCurrent = false;
  }
  m_widgets[m_currentWidget]->draw();
}
void WidgetSet::updateCurrent() {
  m_widgets[m_currentWidget]->update();
}

Widget *WidgetSet::getCurrent() {
  return m_widgets[m_currentWidget];
}

void WidgetSet::changeMode() {
  m_widgets[m_currentWidget]->changeMode();
}

void WidgetSet::setClearScreensOnDrawCurrent() {
  m_clearScreensOnDrawCurrent = true;
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

void WidgetSet::showLoading() {
  // display loading screen here
    m_screenManager->fillAllScreens(TFT_BLACK);
    m_screenManager->selectScreen(2);

    TFT_eSPI &display = m_screenManager->getDisplay();

    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(3);  // Set text size

    // Calculate center positions
    int centre = display.width() / 2;
    display.drawString("Loading Data", centre, centre, 1);
}

void WidgetSet::updateAll() {
  for (int8_t i; i<m_widgetCount; i++) {
    Serial.println("updating widget #" + String(i));
    m_widgets[i]->update();
  }
}

bool WidgetSet::initialUpdateDone() {
  return m_initialized;
}

void WidgetSet::initializeAllWidgetsData() {
  showLoading();
  updateAll();
  m_initialized = true;
}
