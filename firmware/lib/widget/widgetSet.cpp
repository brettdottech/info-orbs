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

void WidgetSet::buttonPressed(uint8_t buttonId) {
  m_widgets[m_currentWidget]->buttonPressed(buttonId);
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

void WidgetSet::showCenteredLine(int screen, int size, String text) {
    m_screenManager->selectScreen(screen);

    TFT_eSPI &display = m_screenManager->getDisplay();

    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(size);  // Set text size
    display.drawCentreString(text, 120, 100, 1);
}


void WidgetSet::showLoading() {
    m_screenManager->fillAllScreens(TFT_BLACK);
    
    WidgetSet::showCenteredLine(2, 3, "Loading Data:");
}

void WidgetSet::updateAll() {
  for (int8_t i; i<m_widgetCount; i++) {
    Serial.printf("updating widget %s\n", m_widgets[i]->getName().c_str());
    showCenteredLine(3, 3, m_widgets[i]->getName().c_str());
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
