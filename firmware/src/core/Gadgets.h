#ifndef GADGETS_H
#define GADGETS_H

#include <Arduino.h>
#include "Settings.h"
#include "Button.h"

struct Gadgets
{
public:
    Gadgets(const Settings& settings);
    void prev();
    void next();
    void buttonPressed(OrbButton whichButton, ButtonState state);
};

#endif