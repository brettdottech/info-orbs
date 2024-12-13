#ifndef ORBS_WIFI_MANAGER_CONFIG_H
#define ORBS_WIFI_MANAGER_CONFIG_H

#include <Arduino.h>

// If we want to override all strings from WiFiManager WebPortal, we can copy the file wm_strings_en.h to our include directory,
// give it a proper name and include it here
// #define WM_STRINGS_FILE "orbs_wifimanager_strings.h" // use this instead of wm_strings_en.h

// Style for parameters
const char HTTP_FORM_PARAM_STYLE_FOR_DIV[] PROGMEM =
    "<style>"
    ".param { display: flex; align-items: center; margin: 0; } "
    ".param label { flex: 0 0 70%; text-align: left; } "
    ".param input { flex: 0 0 30%; text-align: right; } "
    ".param-string { margin: 0; } "
    "</style>";

#endif // ORBS_WIFI_MANAGER_CONFIG_H