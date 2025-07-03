#include "TgBot.h"
TgBot Bot(&WIFIManager);

TaskHandle_t TgBot_task;
void TgBotCode( void * pvParameters ) {
  Bot.BotInit();

  while(true) {
    Bot.Loop();
    delay(1);
  }
}