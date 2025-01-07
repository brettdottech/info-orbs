#include "TaskManager.h"
#include "GlobalResources.h"
#include "Utils.h"
#include <HTTPClient.h>
#include <memory>

TaskManager *TaskManager::instance = nullptr;
QueueHandle_t TaskManager::requestQueue = nullptr;
QueueHandle_t TaskManager::responseQueue = nullptr;
volatile uint32_t TaskManager::activeRequests = 0;
volatile uint32_t TaskManager::maxConcurrentRequests = 0;
int TaskManager::taskParamsCount = 0;

TaskManager::TaskManager() {
    if (!requestQueue) {
        requestQueue = xQueueCreate(REQUEST_QUEUE_SIZE, REQUEST_QUEUE_ITEM_SIZE);
    }
    if (!responseQueue) {
        responseQueue = xQueueCreate(RESPONSE_QUEUE_SIZE, RESPONSE_QUEUE_ITEM_SIZE);
    }
}

TaskManager *TaskManager::getInstance() {
    if (!instance) {
        instance = new TaskManager();
    }
    return instance;
}

bool TaskManager::addTask(std::unique_ptr<Task> task) {
    if (isUrlInQueue(task->url)) {
        return false;
    }

    auto *params = new TaskParams{task->url, task->callback, task->preProcessResponse, task->taskExec};
    taskParamsCount++; // Increment the count
#ifdef TASKMANAGER_DEBUG
    Serial.printf("TaskParams created: %d\n", taskParamsCount);
#endif

    if (xQueueSend(requestQueue, &params, 0) != pdPASS) {
        delete params;
        taskParamsCount--;
#ifdef TASKMANAGER_DEBUG
        Serial.printf("TaskParams deleted (queue full): %d\n", taskParamsCount);
#endif
        return false;
    }

    return true;
}

void TaskManager::processAwaitingTasks() {
    // First check if there are any requests to process
    if (uxQueueMessagesWaiting(requestQueue) == 0) {
        return; // No requests in queue
    }

    if (xSemaphoreTake(taskSemaphore, 0) != pdTRUE) {
        return;
    }

    Utils::setBusy(true);
    activeRequests++;

    if (activeRequests > maxConcurrentRequests) {
        maxConcurrentRequests = activeRequests;
    }
#ifdef TASKMANAGER_DEBUG
    Serial.println("✅ Obtained semaphore");
    Serial.printf("Active requests: %d (Max seen: %d)\n", activeRequests, maxConcurrentRequests);
#endif

    // Get next request
    TaskParams *taskParams = nullptr;
    if (xQueueReceive(requestQueue, &taskParams, 0) != pdPASS) {
        Serial.println("⚠️ Queue empty after size check!");
        activeRequests--;
        Utils::setBusy(false);
        xSemaphoreGive(taskSemaphore);
        return;
    }

#ifdef TASKMANAGER_DEBUG
    Serial.printf("Processing request: %s (Remaining in queue: %d)\n",
                  taskParams->url.c_str(),
                  uxQueueMessagesWaiting(requestQueue));
#endif

    TaskHandle_t taskHandle;
    BaseType_t result = xTaskCreate(
        [](void *params) {
            auto *taskParams = static_cast<TaskParams *>(params);
            taskParams->taskExec();
            delete taskParams; // Ensure cleanup after execution
            taskParamsCount--; // Decrement the count
            // Serial.printf("TaskParams deleted: %d\n", taskParamsCount);

            Utils::setBusy(false);
            xSemaphoreGive(taskSemaphore);
            Serial.println("✅ Released semaphore");
            vTaskDelete(nullptr);
        },
        "TASK_EXEC",
        STACK_SIZE,
        taskParams,
        TASK_PRIORITY,
        &taskHandle);

    if (result != pdPASS) {
        Serial.println("Failed to create HTTP request task");
        delete taskParams; // Ensure the object is deleted if task creation fails
        taskParamsCount--; // Decrement the count if task creation fails
        Serial.printf("TaskParams deleted (task creation failed): %d\n", taskParamsCount);
        Utils::setBusy(false);
        xSemaphoreGive(taskSemaphore);
    }
}

void TaskManager::processTaskResponses() {

#ifdef TASKMANAGER_DEBUG
    // Periodically check for TaskParams leaks
    static unsigned long lastLeakCheck = 0;
    if (millis() - lastLeakCheck > 30000) { // Check every 30 seconds
        TaskManager::checkForLeaks();
        lastLeakCheck = millis();
    }
#endif

    if (uxQueueMessagesWaiting(responseQueue) == 0) {
        return;
    }

    ResponseData *responseData;
    while (xQueueReceive(responseQueue, &responseData, 0) == pdPASS) {
        responseData->callback(responseData->httpCode, responseData->response);
        delete responseData; // Ensure the object is deleted after processing
    }
}

bool TaskManager::isUrlInQueue(const String &url) {
    UBaseType_t queueLength = uxQueueMessagesWaiting(requestQueue);
    for (UBaseType_t i = 0; i < queueLength; i++) {
        TaskParams *taskParams;
        if (xQueuePeek(requestQueue, &taskParams, 0) == pdPASS) {
            if (taskParams->url == url) {
                return true;
            }
            xQueueReceive(requestQueue, &taskParams, 0);
            xQueueSend(requestQueue, &taskParams, 0);
        }
    }
    return false;
}
