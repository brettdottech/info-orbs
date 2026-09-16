#ifndef CUSTOM_IMAGE_WIDGET_H
#define CUSTOM_IMAGE_WIDGET_H

#include "Widget.h"

class CustomImageWidget : public Widget {
public:
    CustomImageWidget(ScreenManager &manager, ConfigManager &config);

    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    static constexpr uint16_t MAX_IMAGE_PAGES = 100;

    void discoverPages();
    bool pageExists(uint16_t pageIndex) const;
    String imagePath(uint16_t pageIndex, uint8_t screenIndex) const;
    void nextPage();

    int m_changeIntervalSec;
    uint16_t m_pageCount = 0;
    uint16_t m_currentPage = 0;
    unsigned long m_lastPageChange = 0;
    bool m_redrawNeeded = true;
};

#endif
