Automatic dimming requires a minor hardware modification to the InfoOrbs circuit. This entails
adding a GL5537 photoresistor and a 22K fixed resistor to the breakout section of the circuit
board. The circuit is shown below. Note that photoresistors vary a lot, so it may be necessary
to experiment with the value of the fixed resistor to get the amount of dimming you want. You
can also try changing the values of AUTO_BRIGHTNESS_MIN and AUTO_BRIGHTNESS_MAX in config.h


                    ^ 3.3V
                    |
                    |
                    \
                    /
                  --\-> GL5537 Photoresistor
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

