#include "Translations.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// THIS FILE IS ONLY FOR GENERAL TRANSLATIONS
// Widget translations should be placed in a separate .h/.cpp in the Widget directory

// All translation variables should start with "t_"

constexpr const char *const t_welcome[LANG_NUM] = {
    "Welcome", // EN
    "Willkommen", // DE
    "Bienvenue" // FR
};

constexpr const char *const t_infoOrbs[LANG_NUM] = {
    "InfoOrbs", // EN
    "InfoOrbs", // DE
    "InfoOrbs" // FR
};

constexpr const char *const t_by[LANG_NUM] = {
    "by", // EN
    "von", // DE
    "par" // FR
};

constexpr const char *const t_brettTech[LANG_NUM] = {
    "brett.tech", // EN
    "brett.tech", // DE
    "brett.tech" // FR
};

constexpr const char *const t_version[LANG_NUM] = {
    "Version", // EN
    "Version", // DE
    "Version" // FR
};

constexpr const char *const t_loadingData[LANG_NUM] = {
    "Loading data:", // EN
    "Lade Daten:", // DE
    "Chargement:" // FR
};

constexpr const char *const t_enableWidget[LANG_NUM] = {
    "Enable Widget", // EN
    "Widget aktivieren", // DE
    "Activer le widget" // FR
};

constexpr const char *const t_timezoneLoc[LANG_NUM] = {
    "Timezone, use one from <a href='https://timezonedb.com/time-zones' target='blank'>this list</a>", // EN
    "Zeitzone, verwenden Sie eine aus <a href='https://timezonedb.com/time-zones' target='blank'>dieser Liste</a>", // DE
    "Fuseau horaire, utilisez-en un de <a href='https://timezonedb.com/time-zones' target='blank'>cette liste</a>" // FR
};

constexpr const char *const t_language[LANG_NUM] = {
    "Language", // EN
    "Sprache", // DE
    "Langue" // FR
};

constexpr const char *const t_widgetCycleDelay[LANG_NUM] = {
    "Automatically cycle widgets every X seconds, set to 0 to disable", // EN
    "Wechseln Sie die Widgets automatisch alle X Sekunden, auf 0 setzen, um zu deaktivieren", // DE
    "Faites défiler les widgets automatiquement toutes les X secondes, définissez sur 0 pour désactiver" // FR
};

constexpr const char *const t_ntpServer[LANG_NUM] = {
    "NTP server", // EN
    "NTP-Server", // DE
    "Serveur NTP" // FR
};

constexpr const char *const t_orbRotation[LANG_NUM] = {
    "Orb rotation", // EN
    "Orb-Rotation", // DE
    "Rotation des Orbes" // FR
};

constexpr const char *const t_orbRot[4][LANG_NUM] = {
    {
        "No rotation", // EN
        "Keine Rotation", // DE
        "Pas de rotation" // FR
    },
    {
        "Rotate 90° clockwise", // EN
        "90° im Uhrzeigersinn", // DE
        "Tourné de 90°" // FR
    },
    {
        "Rotate 180°", // EN
        "180° gedreht", // DE
        "Tourné de 180°" // FR
    },
    {
        "Rotate 270° clockwise", // EN
        "270° im Uhrzeigersinn", // DE
        "Tourné de 270°" // FR
    }};

constexpr const char *const t_nightmode[LANG_NUM] = {
    "Enable Nighttime mode", // EN
    "Nachtmodus aktivieren", // DE
    "Activer le mode nuit" // FR
};

constexpr const char *const t_tftBrightness[LANG_NUM] = {
    "TFT Brightness [0-255]", // EN
    "TFT-Helligkeit [0-255]", // DE
    "Luminosité TFT [0-255]" // FR
};

constexpr const char *const t_dimStartHour[LANG_NUM] = {
    "Nighttime Start [24h format]", // EN
    "Nachtmodus Start [24h-Format]", // DE
    "Début de la nuit [format 24h]" // FR
};

constexpr const char *const t_dimEndHour[LANG_NUM] = {
    "Nighttime End [24h format]", // EN
    "Nachtmodus Ende [24h-Format]", // DE
    "Fin de la nuit [format 24h]" // FR
};

constexpr const char *const t_dimBrightness[LANG_NUM] = {
    "Nighttime Brightness [0-255]", // EN
    "Nachtmodus Helligkeit [0-255]", // DE
    "Luminosité nocturne [0-255]" // FR
};
