#include "ClockTranslations.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// All translation variables for ClockWidget should start with "t_clock"

constexpr Translation t_clockNormal = {
    "Normal clock", // EN
};

constexpr Translation t_clockNixie = {
    "Nixie clock", // EN
};

constexpr Translation t_clockCustom = {
    "Custom clock", // EN
};

constexpr Translation t_clockNotAvailable = {
    "n/a", // EN
};

constexpr Translation t_clockDefaultType = {
    "Default Clock Type (you can also switch types with the middle button)", // EN
};

constexpr Translation t_clockFormat = {
    "Clock Format", // EN
};

constexpr TranslationMulti<3> t_clockFormats = {
    {
        "24h mode", // EN
    },
    {
        "12h mode", // EN
    },
    {
        "12h mode (with am/pm)", // EN
    }};

constexpr Translation t_clockShowSecondTicks = {
    "Show Second Ticks", // EN
};

constexpr Translation t_clockColor = {
    "Clock Color", // EN
};

constexpr Translation t_clockShadowing = {
    "Clock Shadowing", // EN
};

constexpr Translation t_clockShadowColor = {
    "Clock Shadow Color", // EN
};

constexpr Translation t_clockOverrideNixieColor = {
    "Override Nixie color (black=disable)", // EN
};

constexpr Translation t_clockEnable = {
    "Enable", // EN
};

constexpr Translation t_clockSecondsTickColor = {
    "Seconds Tick Color", // EN
};

constexpr Translation t_clockOverrideColor = {
    "Override Color (black=disable)", // EN
};