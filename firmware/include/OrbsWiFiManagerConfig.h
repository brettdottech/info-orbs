#ifndef ORBS_WIFI_MANAGER_CONFIG_H
#define ORBS_WIFI_MANAGER_CONFIG_H

#include <Arduino.h>

// If we want to override all strings from WiFiManager WebPortal, we can copy the file wm_strings_en.h to our include directory,
// give it a proper name and include it here
// #define WM_STRINGS_FILE "orbs_wifimanager_strings.h" // use this instead of wm_strings_en.h

// Style for parameters
const char WEBPORTAL_PARAM_PAGE_START[] = "<h1>InfoOrbs Configuration</h1>";
const char WEBPORTAL_PARAM_PAGE_END[] = "<br><h3><font color='red'>Saving will restart the InfoOrbs to apply the new config.</font></h3>";
const char WEBPORTAL_PARAM_FIELDSET_START[] = "<fieldset>";
const char WEBPORTAL_PARAM_FIELDSET_END[] = "</fieldset>";
const char WEBPORTAL_PARAM_LEGEND_START[] = "<legend>";
const char WEBPORTAL_PARAM_LEGEND_END[] = "</legend>";
const char WEBPORTAL_PARAM_DIV_START[] = "<div class='param'>";
const char WEBPORTAL_PARAM_DIV_STRING_START[] = "<div class='param-string'>";
const char WEBPORTAL_PARAM_DIV_END[] = "</div>";

const char WEBPORTAL_PARAM_STYLE[] =
    "<style>"
    ".param { display: flex; align-items: center; margin: 0; } "
    ".param label { flex: 0 0 70%; text-align: left; } "
    ".param input { flex: 0 0 30%; text-align: right; } "
    ".param-string { margin: 0; } "
    "fieldset { margin: 10px 0px; } "
    "legend { font-size: 1.5rem; font-weight: bold; } "
    "</style>";

#endif // ORBS_WIFI_MANAGER_CONFIG_H