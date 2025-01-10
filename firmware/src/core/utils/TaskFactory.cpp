#include "TaskFactory.h"
#include "GlobalResources.h"
#include "TaskManager.h"
#include "Utils.h"
#include <ArduinoLog.h>
#include <HTTPClient.h>

void TaskFactory::httpGetTask(const String &url, Task::ResponseCallback callback, Task::PreProcessCallback preProcess) {
    Log.noticeln("🔵 Starting HTTP request for: %s", url.c_str());

    {
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure();

        http.begin(client, url);
        http.setTimeout(10000); // 10 second timeout

        int httpCode = http.GET();
        String response;

        if (httpCode > 0) {
            response = http.getString();
        } else {
            Log.errorln("🔴 HTTP request failed, error code: %d", httpCode);
        }

        http.end();
        client.stop();

        // Explicitly reset the objects
        http.~HTTPClient(); // Call the destructor
        new (&http) HTTPClient(); // Reinitialize using placement new
        client.~WiFiClientSecure(); // Call the destructor
        new (&client) WiFiClientSecure(); // Reinitialize using placement new

        if (preProcess) {
            preProcess(httpCode, response);
        }

        auto *responseData = new TaskManager::ResponseData{httpCode, response, callback};

        if (xQueueSend(TaskManager::responseQueue, &responseData, 0) != pdPASS) {
            Log.errorln("Failed to queue response");
            delete responseData; // Ensure cleanup if queueing fails
        }
    }
    TaskManager::activeRequests--;

#ifdef TASKMANAGER_DEBUG
    Log.noticeln("Active requests now: %d", TaskManager::activeRequests);
    UBaseType_t highWater = uxTaskGetStackHighWaterMark(NULL);
    Log.noticeln("Remaining task stack space: %d", highWater);
#endif
}
