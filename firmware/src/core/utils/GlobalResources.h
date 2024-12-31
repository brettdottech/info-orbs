#ifndef GLOBAL_RESOURCES_H
#define GLOBAL_RESOURCES_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t globalTaskSemaphore; // Declare the global semaphore

void initializeGlobalResources(); // Function to initialize global resources

#endif // GLOBAL_RESOURCES_H
