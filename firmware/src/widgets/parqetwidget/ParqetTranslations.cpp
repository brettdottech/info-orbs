#include "ParqetTranslations.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// All translation variables should start with "t_"

constexpr const char *const t_portfolio[LANG_NUM] = {
    "Portfolio", // EN
    "Portfolio", // DE
    "Portefeuille" // FR
};

constexpr const char *const t_total[LANG_NUM] = {
    "T O T A L", // EN
    "G E S A M T", // DE
    "T O T A L" // FR
};

constexpr const char *const t_cnfPortfolioId[LANG_NUM] = {
    "Portfolio ID (must be set to public!)", // EN
    "Portfolio-ID (muss auf öffentlich gesetzt werden!)", // DE
    "ID du portefeuille (doit être défini sur public!)" // FR
};

constexpr const char *const t_cnfMode[LANG_NUM] = {
    "Default timeframe (you can change timeframes by medium pressing the middle button)", // EN
    "Standard-Zeitrahmen (Sie können die Zeitrahmen durch mittellanges Drücken der mittleren Taste ändern)", // DE
    "Plage horaire par défaut (vous pouvez changer les plages horaires en appuyant sur le bouton central)" // FR
};

constexpr const char *const t_cnfPerfMeasure[LANG_NUM] = {
    "Performance measure", // EN
    "Performance-Messwert", // DE
    "Mesure de performance" // FR
};

constexpr const char *const t_cnfChartMeasure[LANG_NUM] = {
    "Chart measure", // EN
    "Chart-Messwert", // DE
    "Mesure du graphique" // FR
};

constexpr const char *const t_cnfClock[LANG_NUM] = {
    "Show clock on first screen", // EN
    "Uhr auf dem ersten Bildschirm anzeigen", // DE
    "Afficher l'horloge sur le premier écran" // FR
};

constexpr const char *const t_cnfTotals[LANG_NUM] = {
    "Show totals screen", // EN
    "Gesamtbildschirm anzeigen", // DE
    "Afficher l'écran total" // FR
};

constexpr const char *const t_cnfTotalVal[LANG_NUM] = {
    "Show total portfolio value", // EN
    "Gesamtportfolio-Wert anzeigen", // DE
    "Afficher la valeur totale du portefeuille" // FR
};

constexpr const char *const t_cnfValues[LANG_NUM] = {
    "Show price or value for stocks", // EN
    "Preis oder Wert für Aktien anzeigen", // DE
    "Afficher le prix ou la valeur des actions" // FR
};

constexpr const char *const t_cnfProxyUrl[LANG_NUM] = {
    "ParqetProxy URL", // EN
    "ParqetProxy-URL", // DE
    "URL ParqetProxy" // FR
};
