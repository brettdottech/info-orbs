#ifndef GLOBAL_RESOURCES_H
#define GLOBAL_RESOURCES_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t taskSemaphore;

void initializeGlobalResources();

#endif // GLOBAL_RESOURCES_H
