#include "WIFIManagerTgBot.h"
WIFIManagerTgBot WIFIManager;

#include "Tasks/TaskWebSocket.h"
#include "Tasks/TaskQueue.h"
#include "Tasks/TaskMonitoring.h"
#include "Tasks/TaskTime.h"
#include "Tasks/TaskTgBot.h"

void setup() 
{
  EEPROM.begin(512);

  WiFi.begin("TP-Link_467D", "66484608");
  uint64_t Timer = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis () > Timer) Timer = millis() + 200;
  }

  WebSocketInit(); 

  xTaskCreatePinnedToCore(QueueCode, "QueueLoop", 1024 * 7, NULL, 6, &Queue_task, 1);

  xTaskCreatePinnedToCore(MonitoringCodeZone1, "MonitoringZone1", 1024 * 20, NULL, 3, &Monitoring_taskZone1, 0);
  
  delay(10);

  xTaskCreatePinnedToCore(TgBotCode, "TgBootLoop", 1024 * 25, NULL, 2, &TgBot_task, 1); 
  xTaskCreatePinnedToCore(TimeCode, "TimeLoop", 1024 * 4, NULL, 5, &Time_task, 1); 
  
}

void loop() 
{
  webSocket.loop();
}


