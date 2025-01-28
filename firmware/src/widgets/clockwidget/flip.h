#ifndef FLIP_H
#define FLIP_H

#include "config_helper.h"
#include <Arduino.h>

#define FLIP_DISABLED 0
#define FLIPS 1

#ifndef USE_CLOCK_FLIP
    #define USE_CLOCK_FLIP FLIP // enable flip by default
#endif

#if defined(USE_CLOCK_FLIP) && (USE_CLOCK_FLIP == FLIPS)
extern const byte *clock_flip[72][2];
#endif

#endif
