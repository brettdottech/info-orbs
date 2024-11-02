#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

enum ButtonState {
	BTN_NOTHING, BTN_SHORT, BTN_MEDIUM, BTN_LONG
};

class Button
{
	public:
		Button(uint8_t pin);
		void begin();
		bool pressedShort();
		bool pressedMedium();
		bool pressedLong();
		ButtonState getState();
		
#if BUTTON_MODE == INPUT_PULLDOWN
		const static bool PRESSED_LEVEL = HIGH;
		const static bool RELEASED_LEVEL = LOW;
#else
		const static bool PRESSED_LEVEL = LOW;
		const static bool RELEASED_LEVEL = HIGH;
#endif

#ifdef BUTTON_DEBOUNCE_TIME
		const static long DEBOUNCE_TIME = BUTTON_DEBOUNCE_TIME;
#else
		const static long DEBOUNCE_TIME = 50;
#endif
#ifdef BUTTON_MEDIUM_PRESS_TIME
		const static long MEDIUM_PRESS_TIME = BUTTON_MEDIUM_PRESS_TIME;
#else
		const static long MEDIUM_PRESS_TIME = 500;
#endif
#ifdef BUTTON_LONG_PRESS_TIME
		const static long LONG_PRESS_TIME = BUTTON_LONG_PRESS_TIME;
#else
		const static long LONG_PRESS_TIME = 2000;
#endif

	private:
		uint8_t  m_pin;
		bool     m_pinLevel;
		ButtonState  m_state;
		bool     m_hasChanged;
		unsigned long m_lastPinLevelChange;
		unsigned long m_pressedSince;

		ButtonState read();
		bool has_changed();
};

#endif // BUTTON_H
