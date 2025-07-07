#include "Tasks/TaskQueue.h"

std::queue<JsonDocument> queue;          
TaskHandle_t Queue_task;          

void QueueCode( void * pvParameters ) {
    while(true) {
        if(!queue.empty()) {
            JsonDocument doc = queue.front();
            String jsonString;
            serializeJson(doc, jsonString);
            webSocket.sendTXT(jsonString);

            queue.pop();
        }

        delay(10);
    }
}