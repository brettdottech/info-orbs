#include "Button.h"
#include "config_helper.h"

/**
 * After calling begin() make sure to attach an interrupt handler in main.cpp that will call isrButtonChange()
 */
Button::Button(uint8_t pin)
    : m_pin(pin), m_lastPinLevelChange(0),
      m_pinLevel(RELEASED_LEVEL), m_state(BTN_NOTHING),
      m_hasChanged(false) {}

void Button::begin() {
    pinMode(m_pin, BUTTON_MODE);
}

void Button::isrButtonChange() {
    if (millis() - m_lastPinLevelChange < DEBOUNCE_TIME) {
        return;
    }

    bool newPinLevel = digitalRead(m_pin);
    if (newPinLevel != m_pinLevel) {
        m_pinLevel = newPinLevel;
        m_lastPinLevelChange = millis();
        if (m_pinLevel == RELEASED_LEVEL) {
            // Button was now released
            // We now check if this was a short, medium or long press
            if (millis() - m_pressedSince >= LONG_PRESS_TIME) {
                m_state = BTN_LONG;
            } else if (millis() - m_pressedSince >= MEDIUM_PRESS_TIME) {
                m_state = BTN_MEDIUM;
            } else {
                m_state = BTN_SHORT;
            }
        } else {
            // Start button press
            m_pressedSince = millis();
            m_state = BTN_NOTHING;
        }
        m_hasChanged = true;
    }
}

bool Button::has_changed() {
    if (m_hasChanged == true) {
        m_hasChanged = false;
        return true;
    }
    return false;
}

bool Button::pressedShort() {
    return (m_state == BTN_SHORT && has_changed());
}

bool Button::pressedMedium() {
    return (m_state == BTN_MEDIUM && has_changed());
}

bool Button::pressedLong() {
    return (m_state == BTN_LONG && has_changed());
}

ButtonState Button::getState() {
    if (has_changed()) {
        return m_state;
    } else {
        return BTN_NOTHING;
    }
}

uint8_t Button::stringToButtonId(const String &buttonName) {
    if (buttonName.equalsIgnoreCase("left")) {
        return BUTTON_LEFT;
    } else if (buttonName.equalsIgnoreCase("middle")) {
        return BUTTON_OK;
    } else if (buttonName.equalsIgnoreCase("right")) {
        return BUTTON_RIGHT;
    } else {
        return 0;
    }
}

ButtonState Button::stringToButtonState(const String &buttonState) {
    if (buttonState.equalsIgnoreCase("short")) {
        return BTN_SHORT;
    } else if (buttonState.equalsIgnoreCase("medium")) {
        return BTN_MEDIUM;
    } else if (buttonState.equalsIgnoreCase("long")) {
        return BTN_LONG;
    } else {
        return BTN_NOTHING;
    }
}
