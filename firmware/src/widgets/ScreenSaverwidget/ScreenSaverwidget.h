#ifndef SCREEN_SAVER_WIDGET_H
#define SCREEN_SAVER_WIDGET_H

#include "widget.h"
#include "config_helper.h"
#include "DigitalRainAnimation.hpp"

class ScreenSaverwidget : public Widget {
public:
    ScreenSaverwidget(ScreenManager &manager, ConfigManager &config);
	~ScreenSaverwidget() override;
	void setup() override;
    void update(bool force) override;
    void draw(bool force) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
//    DigitalRainAnimation<TFT_eSPI> matrix_effect = DigitalRainAnimation<TFT_eSPI>();
    void addConfigToManager();
    bool m_smallFont = false;
    int m_textColor; // = 0x001F;
    int m_headtextColor; // = 0x07FF;
    int m_lineMin = 3; 
    int m_lineMax = 15;
    int m_speedMin = 3; 
    int m_speedMax = 15;
    int m_updateInterval = 100;
};

#endif
