#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H

#include <Arduino.h>
#include "Settings.h"
#include "Button.h"

struct ContentManager
{
public:
    ContentManager(const Settings& settings);
    void prev();
    void next();
    void buttonPressed(OrbButton whichButton, ButtonState state);
};

#endif