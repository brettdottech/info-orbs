#ifndef Button_h
#define Button_h
#include <Arduino.h>

class Button
{
	public:
		Button(uint8_t pin);
		void begin();
		bool read();
		bool toggled();
		bool pressed();
		bool released();
		bool has_changed();
		
		const static bool PRESSED = LOW;
		const static bool RELEASED = HIGH;
	
	private:
		uint8_t  m_pin;
		uint16_t m_delay;
		bool     m_state;
		bool     m_hasChanged;
		uint32_t m_ignoreUntil;
};

#endif
