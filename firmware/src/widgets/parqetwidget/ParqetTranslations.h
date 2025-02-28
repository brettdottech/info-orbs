#ifndef PARQETTRANSLATIONS_H
#define PARQETTRANSLATIONS_H

#include "I18n.h"
#include "ParqetWidget.h"

// All translation variables for ParqetWidget should start with "t_pq"

extern Translation t_pqPortfolio;
extern Translation t_pqTotal;
extern Translation t_pqPortfolioId;
extern Translation t_pqTimeframe;
extern TranslationMulti<PARQET_MODE_COUNT> t_pqTimeframes;
extern Translation t_pqPerfMeasure;
extern TranslationMulti<PARQET_PERF_COUNT> t_pqPerfMeasures;
extern Translation t_pqChartMeasure;
extern TranslationMulti<PARQET_PERF_CHART_COUNT> t_pqPerfChartMeasures;
extern Translation t_pqClock;
extern Translation t_pqTotals;
extern Translation t_pqTotalVal;
extern Translation t_pqShowPriceOrValues;
extern TranslationMulti<2> t_pqShowPriceOrValuesOptions;
extern Translation t_pqProxyUrl;

#endif // PARQETTRANSLATIONS_H
