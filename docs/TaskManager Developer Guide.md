# TaskFactory API Documentation

The `TaskFactory` class is responsible for creating tasks that can be executed by the `TaskManager`. It provides factory methods to create different types of tasks, such as HTTP and MQTT tasks and can be extended for other task types.

## Current Methods

### `createHttpGetTask`

- **Description**: Creates an HTTP task that will perform an HTTP GET request to the specified URL. The task will call the provided callback with the HTTP response.
- **Parameters**:
  - `const String &url`: The URL to which the HTTP request will be made.
  - `Task::ResponseCallback callback`: The callback function that will be called with the HTTP response.
  - `Task::PreProcessCallback preProcess` (optional): A function that will pre-process the response before it is passed to the callback.
- **Returns**: A `std::unique_ptr<Task>` representing the HTTP task.
- **Example**:
  ```cpp
  auto task = TaskFactory::createHttpGetTask(
      "https://example.com/api/data",
      [](int httpCode, const String &response) {
          if (httpCode > 0) {
              Serial.println("Response: " + response);
          } else {
              Serial.println("HTTP request failed");
          }
      }
  );
  TaskManager::getInstance()->addTask(std::move(task));
  ```

---

### `createMqttTask`

- **Description**: Creates an MQTT task. This is currently a placeholder and does not contain any actual MQTT logic yet.
- **Parameters**:
  - `const String &topic`: The MQTT topic to subscribe to or publish to.
  - `Task::ResponseCallback callback`: The callback function that will be called with the MQTT response.
- **Returns**: A `std::unique_ptr<Task>` representing the MQTT task.
- **Example**:
  ```cpp
  auto task = TaskFactory::createMqttTask(
      "home/sensor/temperature",
      [](int httpCode, const String &response) {
          // Handle MQTT response
      }
  );
  TaskManager::getInstance()->addTask(std::move(task));
  ```

# TaskManager API Documentation

The `TaskManager` singleton class is responsible for managing and executing tasks. It uses FreeRTOS Tasks to handle task thread execution and it also FreeRTOS semaphores and queues to ensure only a single task can run at a time and also to gaurd against data race conditions.

## Methods

### `getInstance`

- **Description**: Returns the singleton instance of the `TaskManager`.
- **Returns**: A pointer to the `TaskManager` instance.
- **Example**:
  ```cpp
  TaskManager* manager = TaskManager::getInstance();
  ```

---

### `addTask`

- **Description**: Adds a task to the task queue for execution.
- **Parameters**:
  - `std::unique_ptr<Task> task`: The task to be added to the queue.
- **Returns**: `true` if the task was successfully added to the queue, `false` otherwise.
- **Example**:
  ```cpp
  auto task = TaskFactory::createHttpTask(
      "https://example.com/api/data",
      [](int httpCode, const String &response) {
          if (httpCode > 0) {
              Serial.println("Response: " + response);
          } else {
              Serial.println("HTTP request failed");
          }
      }
  );
  TaskManager::getInstance()->addTask(std::move(task));
  ```

---

### `processAwaitingTasks`

- **Description**: Processes tasks that are waiting in the queue. This method is called from the mainline loop to ensure tasks are executed.
- **Example**:
  ```cpp
  TaskManager::getInstance()->processAwaitingTasks();
  ```

---

### `processTaskResponses`

- **Description**: Processes responses from completed tasks. This method is called from the mainline loop to process task responses.
- **Example**:
  ```cpp
  TaskManager::getInstance()->processTaskResponses();
  ```

---

# Example Usage

See the [WeatherWidget](/firmware/src/widgets/weatherwidget/WeatherWidget.cpp) for an example of how to use TaskFactory and TaskManager. The main steps include:

- Optionally create a preProcess callback method to filter or massage the response data prior to being processed for the widget. Dont update core objects in this callback
- Create a callback method to process the http response data (or filtered data) for the widget
- Call TaskFactory::createHttpGetTask to get a Task object (passing url and callbacks)
- Call TaskManager::getInstance()->addTask to add your Task to the queue for processing
