#include "Tasks/TaskQueue.h"
#include "TgBot.h"
//std::queue<JsonDocument> queue;

QueueHandle_t queue;
TaskHandle_t Queue_task;          
extern TgBot Bot;

void QueueCode( void * pvParameters ) {
    JsonDocument *received_doc;
    while(true) {
        if (xQueueReceive(queue, &received_doc, portMAX_DELAY) == pdPASS) {
            String jsonString;
            serializeJson(*received_doc, jsonString);

            if((*received_doc)["TypeMessage"] == 6) {
                if((*received_doc)["Message"] == 0) Bot.SendMessage("Вода закончилась");
            }
            else webSocket.sendTXT(jsonString);

            delete received_doc;
            /*if(!queue.empty()) {
                JsonDocument doc = queue.front();
                String jsonString;
                serializeJson(doc, jsonString);

                if(doc["TypeMessage"] == 6) {
                    if(doc["Message"] == 0) Bot.SendMessage("Вода закончилась");
                }
                else webSocket.sendTXT(jsonString);

                queue.pop();
            }*/
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}