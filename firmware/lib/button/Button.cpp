#include "Button.h"
#include "config.h"


Button::Button(uint8_t pin)
    : m_pin(pin), m_delay(100), m_ignoreUntil(0),
      m_stateLevel(RELEASED_LEVEL), m_state(STATE_NOTHING),
      m_hasChanged(false) {}

void Button::begin() {
  pinMode(m_pin, BUTTON_MODE);
}

uint8_t Button::read() {
  if (m_ignoreUntil > millis()) {
	  return m_state;
  }

  if (digitalRead(m_pin) != m_stateLevel) {
    m_ignoreUntil = millis() + m_delay;
    m_stateLevel = !m_stateLevel;
    if (m_stateLevel == RELEASED_LEVEL) {
      // Button was now released
      // We now check if this was a long or a short press
      if (millis() - m_pressedSince < LONGPRESS_TIME) {
        m_state = STATE_PRESSED;
      } else {
        m_state = STATE_LONGPRESSED;
      }
    } else {
      // Start button press
      m_pressedSince = millis();
      m_state = STATE_NOTHING;
    }
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
  if (read() == STATE_PRESSED && has_changed() == true) {
    return true;
  } else
    return false;
}

bool Button::longPressed() {
  if (read() == STATE_LONGPRESSED && has_changed() == true)
    return true;
  else
    return false;
}

// bool Button::released() {
//   if (read() == RELEASED && has_changed() == true)
//     return true;
//   else
//     return false;
// }
