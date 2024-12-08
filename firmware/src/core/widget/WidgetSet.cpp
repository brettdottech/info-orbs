#include "WidgetSet.h"

WidgetSet::WidgetSet(ScreenManager *sm, ConfigManager &config) : m_screenManager(sm), m_configManager(config) {
    config.addConfigInt("TFT Settings", "tftBrightness", &m_tftBrightness, "TFT Brightness [0-255]");
    config.addConfigBool("TFT Settings", "nightmode", &m_nightMode, "Enable Nighttime mode");
    String optHours[] = {"0:00", "1:00", "2:00", "3:00", "4:00", "5:00", "6:00", "7:00", "8:00", "9:00", "10:00", "11:00",
                         "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00", "20:00", "21:00", "22:00", "23:00"};
    config.addConfigComboBox("TFT Settings", "dimStartHour", &m_dimStartHour, optHours, 24, "Nighttime Start [24h format]");
    config.addConfigComboBox("TFT Settings", "dimEndHour", &m_dimEndHour, optHours, 24, "Nighttime End [24h format]");
    config.addConfigInt("TFT Settings", "dimBrightness", &m_dimBrightness, "Nighttime Brightness [0-255]");
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
    switchWidget();
}

void WidgetSet::prev() {
    m_currentWidget--;
    if (m_currentWidget < 0) {
        m_currentWidget = m_widgetCount - 1;
    }
    switchWidget();
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
    showCenteredLine(3, "Loading data:");
}

void WidgetSet::updateAll() {
    for (int8_t i; i < m_widgetCount; i++) {
        Serial.printf("updating widget %s\n", m_widgets[i]->getName().c_str());
        showCenteredLine(4, m_widgets[i]->getName());
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

void WidgetSet::updateBrightnessByTime(uint8_t hour24) {
    uint8_t newBrightness;
    if (m_nightMode) {
        bool isInDimRange;
        if (m_dimStartHour < m_dimEndHour) {
            // Normal case: the range does not cross midnight
            isInDimRange = (hour24 >= m_dimStartHour && hour24 < m_dimEndHour);
        } else {
            // Case where the range crosses midnight
            isInDimRange = (hour24 >= m_dimStartHour || hour24 < m_dimEndHour);
        }
        newBrightness = isInDimRange ? m_dimBrightness : m_tftBrightness;
    } else {
        newBrightness = m_tftBrightness;
    }

    if (m_screenManager->setBrightness(newBrightness)) {
        // brightness was changed -> update widget
        m_screenManager->clearAllScreens();
        drawCurrent(true);
    }
}