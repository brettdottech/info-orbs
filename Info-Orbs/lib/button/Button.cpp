#include "Button.h"
#include "config.h"


Button::Button(uint8_t pin)
    : m_pin(pin), m_delay(100), m_ignoreUntil(0),
      m_hasChanged(false) {}

void Button::begin() {
#if BUTTON_MODE == INPUT_PULLDOWN
    m_state = LOW;
#else
    m_state = HIGH;
#endif
    pinMode(m_pin, BUTTON_MODE);
}

bool Button::read() {
  if (m_ignoreUntil > millis()) {
	return m_state;
  }

  if (digitalRead(m_pin) != m_state) {
    m_ignoreUntil = millis() + m_delay;
    m_state = !m_state;
    m_hasChanged = true;
  }

  return m_state;
}

bool Button::toggled() {
  read();
  return has_changed();
}

bool Button::has_changed() {
  if (m_hasChanged == true) {
    m_hasChanged = false;
    return true;
  }
  return false;
}

bool Button::pressed() {
  if (read() == PRESSED && has_changed() == true)
    return true;
  else
    return false;
}

bool Button::released() {
  if (read() == RELEASED && has_changed() == true)
    return true;
  else
    return false;
}
