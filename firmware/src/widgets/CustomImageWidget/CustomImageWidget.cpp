#include "CustomImageWidget.h"

#include <ArduinoLog.h>
#include <LittleFS.h>

CustomImageWidget::CustomImageWidget(
    ScreenManager &manager,
    ConfigManager &config)
    : Widget(manager, config),
      m_changeIntervalSec(CUSTOM_IMAGE_CHANGE_INTERVAL_SEC) {
    m_enabled = (INCLUDE_CUSTOM_IMAGE == WIDGET_ON);

    m_config.addConfigBool(
        "CustomImageWidget",
        "imgEnabled",
        &m_enabled,
        "Enable custom image widget");

    m_config.addConfigInt(
        "CustomImageWidget",
        "imgCycleSec",
        &m_changeIntervalSec,
        "Seconds between custom image pages (0 disables automatic changes)",
        true);
}

void CustomImageWidget::setup() {
    discoverPages();
    if (m_currentPage >= m_pageCount) {
        m_currentPage = 0;
    }
    m_lastPageChange = millis();
    m_redrawNeeded = true;
}

void CustomImageWidget::update(bool force) {
    if (m_pageCount < 2 || m_changeIntervalSec <= 0) {
        return;
    }

    const unsigned long intervalMs =
        static_cast<unsigned long>(m_changeIntervalSec) * 1000UL;
    if (force || millis() - m_lastPageChange >= intervalMs) {
        nextPage();
    }
}

void CustomImageWidget::buttonPressed(
    uint8_t buttonId,
    ButtonState state) {
    if (buttonId == BUTTON_MIDDLE && state == BTN_SHORT) {
        nextPage();
    }
}

String CustomImageWidget::imagePath(
    uint16_t pageIndex,
    uint8_t screenIndex) const {
    // Keep the original filenames as page 1 for backwards compatibility.
    if (pageIndex == 0) {
        return "/CustomImageWidget/screen_" + String(screenIndex + 1) + ".jpg";
    }

    return "/CustomImageWidget/page_" + String(pageIndex + 1) +
           "_screen_" + String(screenIndex + 1) + ".jpg";
}

bool CustomImageWidget::pageExists(uint16_t pageIndex) const {
    for (uint8_t screen = 0; screen < NUM_SCREENS; ++screen) {
        const String path = imagePath(pageIndex, screen);
        if (!LittleFS.exists(path.c_str())) {
            return false;
        }
    }
    return true;
}

void CustomImageWidget::discoverPages() {
    m_pageCount = 0;
    while (m_pageCount < MAX_IMAGE_PAGES && pageExists(m_pageCount)) {
        ++m_pageCount;
    }

    Log.noticeln("Custom image widget found %u complete page(s)", m_pageCount);
    if (m_pageCount == MAX_IMAGE_PAGES) {
        Log.warningln("Custom image page limit (%u) reached", MAX_IMAGE_PAGES);
    }
}

void CustomImageWidget::nextPage() {
    if (m_pageCount == 0) {
        return;
    }

    m_currentPage = static_cast<uint16_t>((m_currentPage + 1) % m_pageCount);
    m_lastPageChange = millis();
    m_redrawNeeded = true;
}

void CustomImageWidget::draw(bool force) {
    if ((!force && !m_redrawNeeded) || m_pageCount == 0) {
        return;
    }

    for (uint8_t screen = 0; screen < NUM_SCREENS; screen++) {
        m_manager.selectScreen(screen);
        m_manager.fillScreen(TFT_BLACK);

        const String path = imagePath(m_currentPage, screen);
        JRESULT result = m_manager.drawFsJpg(0, 0, path.c_str());

        if (result != JDR_OK) {
            Log.warningln(
                "Unable to draw custom image page %u, screen %u",
                m_currentPage + 1,
                screen + 1);
        }
    }

    m_redrawNeeded = false;
}

String CustomImageWidget::getName() {
    return "Custom Images";
}
