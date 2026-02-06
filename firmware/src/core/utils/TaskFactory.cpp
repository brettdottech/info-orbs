#include "TaskFactory.h"
#include "GlobalResources.h"
#include "TaskManager.h"
#include "Utils.h"
#include <ArduinoLog.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

void TaskFactory::httpGetTask(const String &url, Task::ResponseCallback callback, Task::PreProcessCallback preProcess) {
    Log.noticeln("🔵 Starting HTTP request for: %s", url.c_str());

    {
        HTTPClient http;
        bool isHttps = url.startsWith("https://"); // Check if the URL is HTTPS

        // Declare client outside the conditional blocks
        WiFiClient *client = nullptr;
        if (isHttps) {
            client = new WiFiClientSecure();
            static_cast<WiFiClientSecure *>(client)->setInsecure(); // Bypass SSL certificate validation
            http.begin(*client, url); // Use WiFiClientSecure for HTTPS
        } else {
            client = new WiFiClient();
            http.begin(*client, url); // Use WiFiClient for HTTP
        }

        http.setTimeout(10000); // 10-second timeout

        int httpCode = http.GET();
        String response;

        if (httpCode > 0) {
            response = http.getString();
        } else {
            Log.errorln("🔴 HTTP request failed, error code: %d", httpCode);
        }

        http.end();

        // Explicitly reset the objects
        http.~HTTPClient(); // Call the destructor
        new (&http) HTTPClient(); // Reinitialize using placement new

        if (isHttps) {
            static_cast<WiFiClientSecure *>(client)->~WiFiClientSecure(); // Call the destructor
            new (client) WiFiClientSecure(); // Reinitialize using placement new
        } else {
            static_cast<WiFiClient *>(client)->~WiFiClient(); // Call the destructor
            new (client) WiFiClient(); // Reinitialize using placement new
        }

        delete client; // Clean up the client object

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
