#include "GlobalResources.h"

// Define the taskSemaphore as a global variable
SemaphoreHandle_t taskSemaphore = nullptr;

void initializeGlobalResources() {
    // Create the binary semaphore if it hasn't been created yet
    if (!taskSemaphore) {
        taskSemaphore = xSemaphoreCreateBinary();

        // Give the semaphore initially so it can be taken by tasks
        xSemaphoreGive(taskSemaphore);
    }
}

/*
 * Usage of taskSemaphore:
 *
 * This semaphore is intended to be used for any/all Task threads** in the application
 * to ensure that at most only one Task thread can run at a time**. This is useful for
 * conserving system resources, especially in resource-constrained environments like
 * microcontrollers, where running multiple tasks concurrently could lead to excessive
 * memory or CPU usage.
 *
 * 1. **Taking the Semaphore**:
 *    - Use `xSemaphoreTake(taskSemaphore, portMAX_DELAY)` to take the semaphore.
 *    - This will block the task until the semaphore is available.
 *    - Example:
 *      if (xSemaphoreTake(taskSemaphore, portMAX_DELAY) == pdTRUE) {
 *          // Semaphore taken, proceed with the critical section
 *      }
 *
 * 2. **Giving the Semaphore**:
 *    - Use `xSemaphoreGive(taskSemaphore)` to release the semaphore after the critical section.
 *    - Example:
 *      xSemaphoreGive(taskSemaphore); // Release the semaphore
 *
 * 3. **Checking the Semaphore**:
 *    - Use `xSemaphoreTake(taskSemaphore, 0)` to check if the semaphore is available without blocking.
 *    - This is useful for non-blocking checks.
 *    - Example:
 *      if (xSemaphoreTake(taskSemaphore, 0) == pdTRUE) {
 *          // Semaphore is available, proceed
 *          xSemaphoreGive(taskSemaphore); // Release immediately if not needed
 *      } else {
 *          // Semaphore is not available
 *      }
 *
 * Notes:
 * - The semaphore is binary, so it can only be held by one task at a time.
 * - Always ensure the semaphore is released after use to avoid deadlocks.
 * - Use `portMAX_DELAY` for blocking waits or a specific timeout (in ticks) for timed waits.
 * - This semaphore is **global** and should be used by all tasks that need to enforce
 *   single-threaded execution to conserve resources.
 */
