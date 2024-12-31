#include <Arduino.h>
#include "GlobalResources.h"


SemaphoreHandle_t globalTaskSemaphore = nullptr; // Define the global semaphore

void initializeGlobalResources() {
    if (globalTaskSemaphore == nullptr) {
        globalTaskSemaphore = xSemaphoreCreateBinary(); // Create the semaphore
        if (globalTaskSemaphore == nullptr) {
            Serial.println("Failed to create global task semaphore");
            return;
        }
        xSemaphoreGive(globalTaskSemaphore); // Initialize the semaphore as available
    }
}
