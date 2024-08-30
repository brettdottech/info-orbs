#include "widget.h"

Widget::Widget(ScreenManager &manager): m_manager(manager) 
{}

void Widget::setBusy(bool busy) {
    if (busy) {
        digitalWrite(BUSY_PIN, HIGH);
    } else {
        digitalWrite(BUSY_PIN, LOW);
    }
}
