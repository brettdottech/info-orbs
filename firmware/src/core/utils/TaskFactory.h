#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

#include "TaskManager.h"
#include <memory>

class TaskFactory {
public:
    static std::unique_ptr<Task> createHttpGetTask(const String &url, Task::ResponseCallback callback, Task::PreProcessCallback preProcess = nullptr) {
        return std::unique_ptr<Task>(new Task(
            url, callback, [url, callback, preProcess]() { TaskFactory::httpGetTask(url, callback, preProcess); }, preProcess));
    }

    static std::unique_ptr<Task> createMqttTask(const String &topic, Task::ResponseCallback callback) {
        return std::unique_ptr<Task>(new Task(
            topic, callback, []() {
                // Placeholder for MQTT task execution logic
            },
            nullptr));
    }

    // Declare the httpGetTask method
    static void httpGetTask(const String &url, Task::ResponseCallback callback, Task::PreProcessCallback preProcess);
};

#endif // TASK_FACTORY_H
