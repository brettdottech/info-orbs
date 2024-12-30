#include "HTTPClientWrapper.h"
#include "Utils.h"
#include <HTTPClient.h>

HTTPClientWrapper* HTTPClientWrapper::instance = nullptr;
SemaphoreHandle_t HTTPClientWrapper::requestSemaphore = nullptr;
QueueHandle_t HTTPClientWrapper::requestQueue = nullptr;
QueueHandle_t HTTPClientWrapper::responseQueue = nullptr;

// Initialize static debug counters
volatile uint32_t HTTPClientWrapper::activeRequests = 0;
volatile uint32_t HTTPClientWrapper::maxConcurrentRequests = 0;

HTTPClientWrapper::HTTPClientWrapper() {
    if (!requestSemaphore) {
        requestSemaphore = xSemaphoreCreateBinary();
        Utils::setBusy(false);
        xSemaphoreGive(requestSemaphore);
    }
    if (!requestQueue) {
        requestQueue = xQueueCreate(REQUEST_QUEUE_SIZE, REQUEST_QUEUE_ITEM_SIZE);  
    }
    if (!responseQueue) {
        responseQueue = xQueueCreate(RESPONSE_QUEUE_SIZE, RESPONSE_QUEUE_ITEM_SIZE);
    }
}

HTTPClientWrapper* HTTPClientWrapper::getInstance() {
    if (!instance) {
        instance = new HTTPClientWrapper();
    }
    return instance;
}

bool HTTPClientWrapper::addRequest(const String& url, ResponseCallback callback, PreProcessCallback preProcessResponse) {
    auto* params = new RequestParams{url, callback, preProcessResponse};
    
    if (xQueueSend(requestQueue, &params, 0) != pdPASS) {
        delete params;
        Serial.println("Failed to queue HTTP request");
        return false;
    }
    
    Serial.printf("Queued request: %s (Queue length: %d)\n", url.c_str(), uxQueueMessagesWaiting(requestQueue));
    return true;
}

void HTTPClientWrapper::processRequestQueue() {  
    // First check if there are any requests to process
    if (uxQueueMessagesWaiting(requestQueue) == 0) {
        return; // No requests in queue
    }

    // Only try to take semaphore if we have work to do
    if (xSemaphoreTake(requestSemaphore, 0) != pdTRUE) {
        //Serial.println("⚠️ Semaphore blocked - request already in progress");
        return;
    }
    Utils::setBusy(true);
    Serial.println("✅ Obtained semaphore");
    activeRequests++;
    if (activeRequests > maxConcurrentRequests) {
        maxConcurrentRequests = activeRequests;
    }
    Serial.printf("Active requests: %d (Max seen: %d)\n", activeRequests, maxConcurrentRequests);

    // Get next request
    RequestParams* requestParams;
    if (xQueueReceive(requestQueue, &requestParams, 0) != pdPASS) {
        // This should never happen since we checked queue size above
        Serial.println("⚠️ Queue empty after size check!");
        activeRequests--;
        Utils::setBusy(false);
        xSemaphoreGive(requestSemaphore);
        return;
    }

    Serial.printf("Processing request: %s (Remaining in queue: %d)\n", 
                 requestParams->url.c_str(), 
                 uxQueueMessagesWaiting(requestQueue));

    // Create task to handle request
    TaskHandle_t taskHandle;
    BaseType_t result = xTaskCreate(
        httpTask,
        "HTTP_REQ",
        STACK_SIZE,
        requestParams,  // Pass params to task
        TASK_PRIORITY,
        &taskHandle
    );

    if (result != pdPASS) {
        Serial.println("Failed to create HTTP request task");
        delete requestParams;
        Utils::setBusy(false);
        xSemaphoreGive(requestSemaphore);
    }
}

void HTTPClientWrapper::httpTask(void* params) {
    auto* requestParams = static_cast<RequestParams*>(params);
    
    Serial.printf("🔵 Starting HTTP request for: %s\n", requestParams->url.c_str());
    
    {
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure(); 
    
        http.begin(client, requestParams->url);
        http.setTimeout(10000); // 10 second timeout
        
        int httpCode = http.GET();
        String response;
        
        if (httpCode > 0) {
            response = http.getString();
        }
        
        http.end();
        client.stop();

        // Explicitly reset the objects
        http.~HTTPClient(); // Call the destructor
        new (&http) HTTPClient(); // Reinitialize using placement new

        client.~WiFiClientSecure(); // Call the destructor
        new (&client) WiFiClientSecure(); // Reinitialize using placement new
        
        // Call preProcessResponse if provided
        if (requestParams->preProcessResponse) {
            requestParams->preProcessResponse(httpCode, response);
        }

        // Queue the response instead of calling callback directly
        auto* responseData = new ResponseData{
            httpCode,
            response,
            requestParams->callback
        };
        
        if (xQueueSend(responseQueue, &responseData, 0) != pdPASS) {
            Serial.println("Failed to queue response");
            delete responseData;
        }
    }
    
    //UBaseType_t highWater = uxTaskGetStackHighWaterMark(NULL);
    //Serial.print("Remaining task stack space: ");
    //Serial.println(highWater);
    
    Serial.printf("🟢 Completed HTTP request for: %s\n", requestParams->url.c_str());
    activeRequests--;
    Serial.printf("Active requests now: %d\n", activeRequests);
    
    delete requestParams;
    Utils::setBusy(false);
    xSemaphoreGive(requestSemaphore);
    Serial.println("✅ Released semaphore");
    vTaskDelete(nullptr);
}

void HTTPClientWrapper::processResponseQueue() {
    ResponseData* responseData;
    
    // Process all available responses
    while (xQueueReceive(responseQueue, &responseData, 0) == pdPASS) {
        responseData->callback(responseData->httpCode, responseData->response);
        delete responseData;
    }
}
