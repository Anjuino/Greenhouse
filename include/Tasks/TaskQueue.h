#pragma once
#include "Arduino.h"
#include <queue>
#include "ArduinoJson.h"
#include <WebSocketsClient.h>
extern WebSocketsClient webSocket;

typedef enum {
    Log,
    GroundHumidity,
    AirHumidity,
    StatePump,
    StateHumidifer,
    StateLamp
} TypeMessage;

extern std::queue<JsonDocument> queue;    
extern TaskHandle_t Queue_task;     

void QueueCode( void * pvParameters );