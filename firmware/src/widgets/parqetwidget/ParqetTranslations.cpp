#include "ParqetTranslations.h"

#include "ParqetWidget.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// All translation variables for ParqetWidget should start with "t_pq"

constexpr Translation t_pqPortfolio = {
    "Portfolio", // EN
    "Portfolio", // DE
    "Portefeuille" // FR
};

constexpr Translation t_pqTotal = {
    "T O T A L", // EN
    "G E S A M T", // DE
    "T O T A L" // FR
};

constexpr Translation t_pqPortfolioId = {
    "Portfolio ID (must be set to public!)", // EN
    "Portfolio-ID (muss auf öffentlich gesetzt werden!)", // DE
    "ID du portefeuille (doit être défini sur public!)" // FR
};

constexpr Translation t_pqTimeframe = {
    "Default timeframe (you can change the timeframe by medium pressing the middle button)", // EN
    "Standard-Zeitrahmen (Sie können den Zeitrahmen durch mittellanges Drücken der mittleren Taste ändern)", // DE
    "Plage horaire par défaut (vous pouvez changer le plage horaire en appuyant sur le bouton central)" // FR
};

constexpr Translation t_pqPerfMeasure = {
    "Performance measure", // EN
    "Performance-Messwert", // DE
    "Mesure de performance" // FR
};

constexpr Translation t_pqChartMeasure = {
    "Chart measure", // EN
    "Chart-Messwert", // DE
    "Mesure du graphique" // FR
};

constexpr Translation t_pqClock = {
    "Show clock on first screen", // EN
    "Uhr auf dem ersten Bildschirm anzeigen", // DE
    "Afficher l'horloge sur le premier écran" // FR
};

constexpr Translation t_pqTotals = {
    "Show totals screen", // EN
    "Gesamtbildschirm anzeigen", // DE
    "Afficher l'écran total" // FR
};

constexpr Translation t_pqTotalVal = {
    "Show total portfolio value", // EN
    "Gesamtportfolio-Wert anzeigen", // DE
    "Afficher la valeur totale du portefeuille" // FR
};

constexpr Translation t_pqShowPriceOrValues = {
    "Show price or value for stocks", // EN
    "Preis oder Wert für Aktien anzeigen", // DE
    "Afficher le prix ou la valeur des actions" // FR
};

constexpr TranslationMulti<2> t_pqShowPriceOrValuesOptions = {
    {
        "Price", // EN
        "Preis", // DE
        "Prix" // FR
    },
    {
        "Value", // EN
        "Wert", // DE
        "Valeur" // FR
    }};

constexpr Translation t_pqProxyUrl = {
    "ParqetProxy URL", // EN
    "ParqetProxy-URL", // DE
    "URL ParqetProxy" // FR
};

constexpr TranslationMulti<PARQET_MODE_COUNT> t_pqTimeframes = {
    {
        "today", // EN
        "heute", // DE
        "aujourd'hui" // FR
    },
    {
        "1 week", // EN
        "1 Woche", // DE
        "1 semaine" // FR
    },
    {
        "1 month", // EN
        "1 Monat", // DE
        "1 mois" // FR
    },
    {
        "3 months", // EN
        "3 Monate", // DE
        "3 mois" // FR
    },
    {
        "6 months", // EN
        "6 Monate", // DE
        "6 mois" // FR
    },
    {
        "1 year", // EN
        "1 Jahr", // DE
        "1 an" // FR
    },
    {
        "3 years", // EN
        "3 Jahre", // DE
        "3 ans" // FR
    },
    {
        "this month", // EN
        "diesen Monat", // DE
        "ce mois" // FR
    },
    {
        "this year", // EN
        "dieses Jahr", // DE
        "cette année" // FR
    },
    {
        "max", // EN
        "max", // DE
        "max" // FR
    }};

constexpr TranslationMulti<PARQET_PERF_COUNT> t_pqPerfMeasures = {
    {
        "Total return gross", // EN
        "Gesamtrendite brutto", // DE
        "Rendement total brut" // FR
    },
    {
        "Total return net", // EN
        "Gesamtrendite netto", // DE
        "Rendement total net" // FR
    },
    {
        "Return gross", // EN
        "Rendite brutto", // DE
        "Rendement brut" // FR
    },
    {
        "Return net", // EN
        "Rendite netto", // DE
        "Rendement net" // FR
    },
    {
        "TTWROR", // EN
        "TTWROR", // DE
        "TTWROR" // FR
    },
    {
        "IRR", // EN
        "IZF", // DE
    }};

constexpr TranslationMulti<PARQET_PERF_CHART_COUNT> t_pqPerfChartMeasures = {
    {
        "Include realized", // EN
        "Realisierte einbeziehen", // DE
        "Inclure réalisé" // FR
    },
    {
        "Only unrealized", // EN
        "Nur nicht realisierte", // DE
        "Uniquement non réalisé" // FR
    },
    {
        "TTWROR", // EN
        "TTWROR", // DE
        "TTWROR" // FR
    },
    {
        "Drawdown", // EN
        "Drawdown", // DE
        "Drawdown" // FR
    }};
