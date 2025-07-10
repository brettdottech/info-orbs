Automatic dimming requires a minor hardware modification to the InfoOrbs circuit. This entails
adding a photoresistor and a 22K fixed resistor to the breakout section of the circuit board. The
circuit is shown below. The photoresistor used in the prototype measured about 200 ohms in bright
sunlight, and about 10 Mohms in darkness. It was obtained from an Elegoo arduino starter kit. It is
probably a 5549, but photoresistors vary a lot, so the value of the fixed resistor may need to
be adjusted proportionally to compensate for a photoresistor with different characteristics.

Be aware that small values for the fixed resistor will draw more current and could damage
the resistor or the board. Don't go below about 1K unless you know what you are doing.

                    ^ 3.3V
                    |
                    |
                    \
                    /
                  --\-> Photoresistor
                    /
                    \
                    |
                    |
                    +--------[GPIO34>
                    |
                    |
                    \
                    /
                    \  22K Fixed Resistor
                    /
                    \
                    |
                    |
                   GND

