#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <functional>
#include <memory>

// Forward declaration of TaskManager to avoid circular dependencies
class TaskManager;

// Define the Task class
class Task {
public:
    using ResponseCallback = std::function<void(int httpCode, const String &response)>;
    using PreProcessCallback = std::function<void(int httpCode, String &response)>;
    using TaskExecCallback = std::function<void()>; // No parameters

    Task(const String &url, ResponseCallback callback, TaskExecCallback taskExec, PreProcessCallback preProcess = nullptr)
        : url(url), callback(callback), preProcessResponse(preProcess), taskExec(taskExec) {}

    String url;
    ResponseCallback callback;
    PreProcessCallback preProcessResponse;
    TaskExecCallback taskExec; // Required

    // Virtual destructor for proper cleanup
    virtual ~Task() = default;
};

class TaskManager {
public:
    using ResponseCallback = Task::ResponseCallback;
    using PreProcessCallback = Task::PreProcessCallback;
    using TaskExecCallback = Task::TaskExecCallback;

    struct TaskParams {
        String url;
        ResponseCallback callback;
        PreProcessCallback preProcessResponse;
        TaskExecCallback taskExec; // Required
    };

    // Make ResponseData public
    struct ResponseData {
        int httpCode;
        String response;
        ResponseCallback callback;
    };

    static TaskManager *getInstance();
    bool addTask(std::unique_ptr<Task> task);
    void processAwaitingTasks();
    void processTaskResponses();

    // Declare static members as extern
    static volatile uint32_t activeRequests;
    static volatile uint32_t maxConcurrentRequests;
    static QueueHandle_t requestQueue;
    static QueueHandle_t responseQueue;

    // Add destructor
    ~TaskManager() {
        // Clean up queues
        vQueueDelete(requestQueue);
        vQueueDelete(responseQueue);
    }

    // Add a debug function to check for leaks
    static void checkForLeaks() {
        Serial.printf("Current TaskParams count: %d\n", taskParamsCount);
        if (taskParamsCount != 0) {
            Serial.println("⚠️ Potential memory leak detected!");
        }
    }

private:
    TaskManager();

    static TaskManager *instance;

    static const uint16_t STACK_SIZE = 6000;
    static const UBaseType_t TASK_PRIORITY = 1;
    static const UBaseType_t REQUEST_QUEUE_SIZE = 10;
    static const UBaseType_t REQUEST_QUEUE_ITEM_SIZE = sizeof(TaskParams *);
    static const UBaseType_t RESPONSE_QUEUE_SIZE = 10;
    static const UBaseType_t RESPONSE_QUEUE_ITEM_SIZE = sizeof(ResponseData *);
    static const TickType_t QUEUE_CHECK_DELAY = pdMS_TO_TICKS(100); // 100ms between queue checks
    bool isUrlInQueue(const String &url);
    static int taskParamsCount;
};

#endif // TASK_MANAGER_H
