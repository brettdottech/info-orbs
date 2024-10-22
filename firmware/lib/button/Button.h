#ifndef Button_h
#define Button_h
#include <Arduino.h>

class Button
{
	public:
		Button(uint8_t pin);
		void begin();
		uint8_t read();
		bool toggled();
		bool pressed();
		bool longPressed();
		// bool released();
		bool has_changed();
		
#if BUTTON_MODE == INPUT_PULLDOWN
		const static bool PRESSED_LEVEL = HIGH;
		const static bool RELEASED_LEVEL = LOW;
#else
		const static bool PRESSED_LEVEL = LOW;
		const static bool RELEASED_LEVEL = HIGH;
#endif

		const static uint8_t STATE_NOTHING = 0;
		const static uint8_t STATE_PRESSED = 1;
		const static uint8_t STATE_LONGPRESSED = 2;

		const static long LONGPRESS_TIME = 500;

	private:
		uint8_t  m_pin;
		uint16_t m_delay;
		bool     m_stateLevel;
		uint8_t  m_state;
		bool     m_hasChanged;
		uint32_t m_ignoreUntil;
		unsigned long m_pressedSince;
};

#endif
