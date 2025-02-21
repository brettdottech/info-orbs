#include "ScreenSaverwidget.h"

ScreenSaverwidget::ScreenSaverwidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config){
    m_enabled = true; 
    m_config.addConfigBool("ScreenSaverWidget", "SmallFont", &m_smallFont, "Small font (unchecked)", false);
    m_config.addConfigColor("ScreenSaverWidget", "TextColor", &m_textColor, "Text Color", false);
    m_config.addConfigColor("ScreenSaverWidget", "HeadTextColor", &m_headtextColor, "Head Text Color", false);
    m_config.addConfigInt("ScreenSaverWidget", "lineMin", &m_lineMin, "Minimum line length", true);
    m_config.addConfigInt("ScreenSaverWidget", "lineMax", &m_lineMax, "Maximum line length", true);
    m_config.addConfigInt("ScreenSaverWidget", "speedMin", &m_speedMin, "Minimum line speed", true);
    m_config.addConfigInt("ScreenSaverWidget", "speedMax", &m_speedMax, "Minimum line speed", true);
    m_config.addConfigInt("ScreenSaverWidget", "updateInterval", &m_updateInterval, "Screen Update Interval", true);
}

void ScreenSaverwidget::addConfigToManager(){
}

ScreenSaverwidget::~ScreenSaverwidget() {
}

void ScreenSaverwidget::setup(){
    int R;
    int G;
    int B;

    m_manager.m_matrix_effect.setup(m_lineMin, m_lineMax, m_speedMin, m_speedMax, m_updateInterval);

    R = ((m_textColor >> 11) & 0x1F)*255/31; 
    G = ((m_textColor >> 5)  & 0x3F)*255/63; 
    B = ( m_textColor        & 0x1F)*255/31;
    m_manager.m_matrix_effect.setTextColor(R, G, B);

    R = ((m_headtextColor >> 11) & 0x1F)*255/31; 
    G = ((m_headtextColor >> 5)  & 0x3F)*255/63; 
    B = ( m_headtextColor        & 0x1F)*255/31;
    m_manager.m_matrix_effect.setHeadCharColor(R, G, B);
}

void ScreenSaverwidget::update(bool force){
}

void ScreenSaverwidget::draw(bool force){
    m_manager.selectAllScreens();
	m_manager.m_matrix_effect.loop();
}

void ScreenSaverwidget::buttonPressed(uint8_t buttonId, ButtonState state){
}

String ScreenSaverwidget::getName(){
    return "Screen Saver";
}