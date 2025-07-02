#include "WIFIManagerTgBot.h"
WIFIManagerTgBot WIFIManager;

#include "Tasks/TaskMonitoring.h"
#include "Tasks/TaskTgBot.h"

void setup() 
{
  Serial.begin(9600);
  EEPROM.begin(512);

  xTaskCreatePinnedToCore(MonitoringCodeZone1, "MonitoringZone1", 1024 * 30, NULL, 3, &Monitoring_taskZone1, 0);
  /*
  delay(10);

  WIFIManager.Start(false);

  delay(50);

  Bot.BotInit();

  xTaskCreatePinnedToCore(TgBotCode, "TgBootLoop", 1024 * 10, NULL, 4, &TgBot_task, 0); 
  */
}

void loop() 
{
  
}


