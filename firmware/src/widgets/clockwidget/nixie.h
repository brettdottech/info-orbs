#ifndef NIXIE_H
#define NIXIE_H

#include "config_helper.h"
#include <Arduino.h>

#define NIXIE_HOLES 1
#define NIXIE_NOHOLES 2

#ifndef USE_CLOCK_NIXIE
    #define USE_CLOCK_NIXIE NIXIE_NOHOLES // enable nixie without holes by default
#endif

#if defined(USE_CLOCK_NIXIE) && (USE_CLOCK_NIXIE == NIXIE_HOLES || USE_CLOCK_NIXIE == NIXIE_NOHOLES)
extern const byte *clock_nixie[12][2];
#endif

#endif
