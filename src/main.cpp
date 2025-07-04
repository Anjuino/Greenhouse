#include "WIFIManagerTgBot.h"
WIFIManagerTgBot WIFIManager;

#include "Tasks/TaskMonitoring.h"
#include "Tasks/TaskTime.h"
#include "Tasks/TaskTgBot.h"

void setup() 
{
  Serial.begin(9600);
  EEPROM.begin(512);

  xTaskCreatePinnedToCore(MonitoringCodeZone1, "MonitoringZone1", 1024 * 30, NULL, 3, &Monitoring_taskZone1, 0);
  
  delay(10);

  WiFi.begin("Wentor", "0hqWs40IsY");

  delay(10);

  xTaskCreatePinnedToCore(TgBotCode, "TgBootLoop", 1024 * 20, NULL, 4, &TgBot_task, 0); 
  xTaskCreatePinnedToCore(TimeCode, "TimeLoop", 1024 * 5, NULL, 5, &Time_task, 1); 
  
}

void loop() 
{
  
}


