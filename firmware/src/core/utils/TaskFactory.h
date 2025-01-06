#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

#include "TaskManager.h"
#include <memory>

// Implementation of make_unique for older C++ standards
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class TaskFactory {
public:
    static std::unique_ptr<Task> createHttpTask(const String &url, Task::ResponseCallback callback, Task::PreProcessCallback preProcess = nullptr) {
        return make_unique<Task>(url, callback, [url, callback, preProcess]() { TaskFactory::httpTask(url, callback, preProcess); }, preProcess);
    }

    static std::unique_ptr<Task> createMqttTask(const String &topic, Task::ResponseCallback callback) {
        return make_unique<Task>(topic, callback, []() {
            // Placeholder for MQTT task execution logic
        },
                                 nullptr);
    }

    // Declare the httpTask method
    static void httpTask(const String &url, Task::ResponseCallback callback, Task::PreProcessCallback preProcess);
};

#endif // TASK_FACTORY_H
