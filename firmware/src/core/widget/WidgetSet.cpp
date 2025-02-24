#include "WidgetSet.h"

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

Widget **WidgetSet::getAllWidgets() {
    return m_widgets;
}

uint8_t WidgetSet::getWidgetCount() const {
    return m_widgetCount;
}

void WidgetSet::drawCurrent(bool force) {
    if (m_clearScreensOnDrawCurrent) {
        m_screenManager->clearAllScreens();
        m_clearScreensOnDrawCurrent = false;
        m_widgets[m_currentWidget]->draw(true);
    } else {
        m_widgets[m_currentWidget]->draw(force);
    }
}

void WidgetSet::updateCurrent() {
    m_widgets[m_currentWidget]->update();
}

Widget *WidgetSet::getCurrent() {
    return m_widgets[m_currentWidget];
}

void WidgetSet::buttonPressed(uint8_t buttonId, ButtonState state) {
    m_widgets[m_currentWidget]->buttonPressed(buttonId, state);
}

void WidgetSet::setClearScreensOnDrawCurrent() {
    m_clearScreensOnDrawCurrent = true;
}

void WidgetSet::next() {
    m_currentWidget++;
    if (m_currentWidget >= m_widgetCount) {
        m_currentWidget = 0;
    }
    if (!getCurrent()->isEnabled()) {
        // Recursive call to next()
        next();
    } else {
        switchWidget();
    }
}

void WidgetSet::prev() {
    if (m_currentWidget == 0) {
        m_currentWidget = m_widgetCount - 1;
    } else {
        m_currentWidget--;
    }
    if (!getCurrent()->isEnabled()) {
        // Recursive call to next()
        prev();
    } else {
        switchWidget();
    }
}

void WidgetSet::switchWidget() {
    m_screenManager->clearAllScreens();
    getCurrent()->setup();
    uint32_t start = millis();
    getCurrent()->draw(true);
    uint32_t end = millis();
    Serial.printf("Drawing of %s took %d ms\n", getCurrent()->getName().c_str(), (end - start));
}

void WidgetSet::showCenteredLine(int screen, const String &text) {
    m_screenManager->selectScreen(screen);
    m_screenManager->fillScreen(TFT_BLACK);
    m_screenManager->setFontColor(TFT_WHITE);
    m_screenManager->drawCentreString(text, ScreenCenterX, ScreenCenterY, 22);
}

void WidgetSet::showLoading() {
    showCenteredLine(3, I18n::get("loading_data"));
}

void WidgetSet::updateAll() {
    for (uint8_t i = 0; i < m_widgetCount; i++) {
        if (m_widgets[i]->isEnabled()) {
            Serial.printf("updating widget %s\n", m_widgets[i]->getName().c_str());
            showCenteredLine(4, m_widgets[i]->getName());
            m_widgets[i]->update();
        }
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
