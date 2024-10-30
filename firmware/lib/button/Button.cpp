#include "Button.h"
#include "config.h"


Button::Button(uint8_t pin)
    : m_pin(pin), m_lastStateLevelChange(0),
      m_stateLevel(RELEASED_LEVEL), m_state(BTN_NOTHING),
      m_hasChanged(false) {}

void Button::begin() {
  pinMode(m_pin, BUTTON_MODE);
}

ButtonState Button::read() {
  if (millis() - m_lastStateLevelChange < DEBOUNCE_TIME) {
    // State change still within debounce time -> ignore
	  return m_state;
  }

  if (digitalRead(m_pin) != m_stateLevel) {
    m_stateLevel = !m_stateLevel;
    m_lastStateLevelChange = millis();
    if (m_stateLevel == RELEASED_LEVEL) {
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
  return m_state;
}

bool Button::has_changed() {
  if (m_hasChanged == true) {
    m_hasChanged = false;
    return true;
  }
  return false;
}

bool Button::pressedShort() {
  return (read() == BTN_SHORT && has_changed());
}

bool Button::pressedMedium() {
  return (read() == BTN_MEDIUM && has_changed());
}

bool Button::pressedLong() {
  return (read() == BTN_LONG && has_changed());
}

ButtonState Button::getState() {
  ButtonState state = read();
  if (has_changed()) {
    return state;
  } else {
    return BTN_NOTHING;
  }
}