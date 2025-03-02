#include "Translations.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// THIS FILE IS ONLY FOR GENERAL TRANSLATIONS
// Widget translations should be placed in a separate .h/.cpp in the Widget directory

// All translation variables should start with "t_"

constexpr Translation t_welcome = {
    "Welcome", // EN
    "Willkommen", // DE
    "Bienvenue" // FR
};

constexpr Translation t_infoOrbs = {
    "InfoOrbs", // EN
    "InfoOrbs", // DE
    "InfoOrbs" // FR
};

constexpr Translation t_by = {
    "by", // EN
    "von", // DE
    "par" // FR
};

constexpr Translation t_brettTech = {
    "brett.tech", // EN
    "brett.tech", // DE
    "brett.tech" // FR
};

constexpr Translation t_version = {
    "Version", // EN
    "Version", // DE
    "Version" // FR
};

constexpr Translation t_loadingData = {
    "Loading data:", // EN
    "Lade Daten:", // DE
    "Chargement:" // FR
};

constexpr Translation t_enableWidget = {
    "Enable Widget", // EN
    "Widget aktivieren", // DE
    "Activer le widget" // FR
};

constexpr Translation t_timezoneLoc = {
    "Timezone, use one from <a href='https://timezonedb.com/time-zones' target='blank'>this list</a>", // EN
    "Zeitzone, verwenden Sie eine aus <a href='https://timezonedb.com/time-zones' target='blank'>dieser Liste</a>", // DE
    "Fuseau horaire, utilisez-en un de <a href='https://timezonedb.com/time-zones' target='blank'>cette liste</a>" // FR
};

constexpr Translation t_language = {
    "Language", // EN
    "Sprache", // DE
    "Langue" // FR
};

constexpr Translation t_widgetCycleDelay = {
    "Automatically cycle widgets every X seconds, set to 0 to disable", // EN
    "Wechseln Sie die Widgets automatisch alle X Sekunden, auf 0 setzen, um zu deaktivieren", // DE
    "Faites défiler les widgets automatiquement toutes les X secondes, définissez sur 0 pour désactiver" // FR
};

constexpr Translation t_ntpServer = {
    "NTP server", // EN
    "NTP-Server", // DE
    "Serveur NTP" // FR
};

constexpr Translation t_orbRotation = {
    "Orb rotation", // EN
    "Orb-Drehung", // DE
    "Rotation des Orbes" // FR
};

constexpr TranslationMulti<4> t_orbRot = {
    {
        "No rotation", // EN
        "Keine Drehung", // DE
        "Pas de rotation" // FR
    },
    {
        "Rotate 90°", // EN
        "90° gedreht", // DE
        "Tourné de 90°" // FR
    },
    {
        "Rotate 180°", // EN
        "180° gedreht", // DE
        "Tourné de 180°" // FR
    },
    {
        "Rotate 270°", // EN
        "270° gedreht", // DE
        "Tourné de 270°" // FR
    }};

constexpr Translation t_nightmode = {
    "Enable Nighttime mode", // EN
    "Nachtmodus aktivieren", // DE
    "Activer le mode nuit" // FR
};

constexpr Translation t_tftBrightness = {
    "TFT Brightness [0-255]", // EN
    "TFT-Helligkeit [0-255]", // DE
    "Luminosité TFT [0-255]" // FR
};

constexpr Translation t_dimStartHour = {
    "Nighttime Start [24h format]", // EN
    "Nachtmodus Start [24h-Format]", // DE
    "Début de la nuit [format 24h]" // FR
};

constexpr Translation t_dimEndHour = {
    "Nighttime End [24h format]", // EN
    "Nachtmodus Ende [24h-Format]", // DE
    "Fin de la nuit [format 24h]" // FR
};

constexpr Translation t_dimBrightness = {
    "Nighttime Brightness [0-255]", // EN
    "Nachtmodus Helligkeit [0-255]", // DE
    "Luminosité nocturne [0-255]" // FR
};

constexpr Translation t_high = {
    "High", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr Translation t_low = {
    "Low", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr Translation t_highShort = {
    "H", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr Translation t_lowShort = {
    "L", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr Translation t_temperatureUnit = {
    "Temperature Unit", // EN
};

constexpr TranslationMulti<2> t_temperatureUnits = {
    {
        "Celsius", // EN
    },
    {
        "Fahrenheit", // EN
    }};

constexpr Translation t_screenMode = {
    "Screen Mode", // EN
};

constexpr TranslationMulti<2> t_screenModes = {
    {
        "Light", // EN
    },
    {
        "Dark", // EN
    }};
