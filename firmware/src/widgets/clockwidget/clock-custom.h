#ifndef CLOCK_CUSTOM_H
#define CLOCK_CUSTOM_H

#include "config_helper.h"
#include <Arduino.h>

#ifndef USE_CLOCK_CUSTOM
    #define USE_CLOCK_CUSTOM 0 // no custom clocks by default
#endif

#if USE_CLOCK_CUSTOM > 0
extern const byte *clock_custom[10][12][2];
#endif

#endif
