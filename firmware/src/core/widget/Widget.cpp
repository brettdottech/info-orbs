#include "Widget.h"

Widget::Widget(ScreenManager &manager, ConfigManager &config) : m_manager(manager), m_config(config) {}

void Widget::setBusy(bool busy) {
    if (busy) {
        digitalWrite(BUSY_PIN, HIGH);
    } else {
        digitalWrite(BUSY_PIN, LOW);
    }
}

bool Widget::isEnabled() {
    return m_enabled;
}
