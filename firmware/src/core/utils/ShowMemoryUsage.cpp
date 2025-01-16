#include "ShowMemoryUsage.h"

#include "config_helper.h"
#include <Arduino.h>

// initialize static members
static unsigned long s_lastMemoryUsageShownAt = 0;

#ifdef MEMORY_DEBUG_INTERVAL
static const unsigned long interval_ms = MEMORY_DEBUG_INTERVAL;
#else
static const unsigned long interval_ms = 5000; // default is needed for cases of where this method is called without MEMORY_DEBUG_INTERVAL being set
#endif

void ShowMemoryUsage::printSerial(bool force, bool newLine) {
    multi_heap_info_t info;
    if (force || (!s_lastMemoryUsageShownAt || (millis() - s_lastMemoryUsageShownAt >= interval_ms))) {
        heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        size_t total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        Serial.printf("total: %d, allocated: %d, totalFree: %d, minFree: %d, largestFree: %d%s", total, info.total_allocated_bytes, info.total_free_bytes, info.minimum_free_bytes, info.largest_free_block, newLine ? "\n" : "");
        s_lastMemoryUsageShownAt = millis();
    }
}
