#include "Tasks/TaskQueue.h"
#include "TgBot.h"
std::queue<JsonDocument> queue;          
TaskHandle_t Queue_task;          
extern TgBot Bot;

void QueueCode( void * pvParameters ) {
    while(true) {
        if(!queue.empty()) {
            JsonDocument doc = queue.front();
            String jsonString;
            serializeJson(doc, jsonString);

            if(doc["TypeMessage"] == 6) {
                if(doc["Message"] == 0) {
                    Bot.SendMessage("Вода закончилась");
                }
            }
            else {
                String jsonString;
                serializeJson(doc, jsonString);
                webSocket.sendTXT(jsonString);
            }

            queue.pop();
        }

        delay(10);
    }
}