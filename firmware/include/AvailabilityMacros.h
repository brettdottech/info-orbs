/*
    We include a dummy version of this file to make OpenFontRender happy.
    It might otherwise show an error message when compiling on Mac computers because it can't
    find AvailabilityMacros.h (referenced in OpenFontRender/freetype/config/ftconfig.h)

    See also https://github.com/brettdottech/info-orbs/pull/130
*/

#ifndef __AVAILABILITYMACROS__
#define __AVAILABILITYMACROS__

#ifndef MAC_OS_X_VERSION_10_4
    #define MAC_OS_X_VERSION_10_4 1 // dummy value
#endif

#ifndef MAC_OS_X_VERSION_MIN_REQUIRED
    #define MAC_OS_X_VERSION_MIN_REQUIRED 1 // dummy value
#endif

#endif