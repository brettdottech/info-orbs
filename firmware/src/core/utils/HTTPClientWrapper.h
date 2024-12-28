#ifndef HTTP_CLIENT_WRAPPER_H
#define HTTP_CLIENT_WRAPPER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <functional>

class HTTPClientWrapper {
public:
    using ResponseCallback = std::function<void(int httpCode, const String& response)>;
    
    static HTTPClientWrapper* getInstance();
    bool makeRequest(const String& url, ResponseCallback callback);
    bool addRequest(const String& url, ResponseCallback callback);
    void processRequestQueue();  
    void processResponseQueue();

private:
    // Add debug counters
    static volatile uint32_t activeRequests;
    static volatile uint32_t maxConcurrentRequests;

    HTTPClientWrapper();
    ~HTTPClientWrapper();
    
    static void httpTask(void* params);
    
    struct RequestParams {
        String url;
        ResponseCallback callback;
    };

    struct ResponseData {
        int httpCode;
        String response;
        ResponseCallback callback;
    };

    static HTTPClientWrapper* instance;
    static SemaphoreHandle_t requestSemaphore;
    static QueueHandle_t requestQueue;
    static QueueHandle_t responseQueue;
    
    static const uint16_t STACK_SIZE = 8192;
    static const UBaseType_t TASK_PRIORITY = 1;
    static const UBaseType_t REQUEST_QUEUE_SIZE = 20;  
    static const UBaseType_t REQUEST_QUEUE_ITEM_SIZE = sizeof(RequestParams*);  
    static const UBaseType_t RESPONSE_QUEUE_SIZE = 20;
    static const UBaseType_t RESPONSE_QUEUE_ITEM_SIZE = sizeof(ResponseData*);
    static const TickType_t QUEUE_CHECK_DELAY = pdMS_TO_TICKS(100); // 100ms between queue checks
};

#endif // HTTP_CLIENT_WRAPPER_H
