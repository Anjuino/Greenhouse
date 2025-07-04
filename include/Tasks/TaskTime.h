#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

extern class DeviceGreenhous Zone1;

TaskHandle_t Time_task;
void TimeCode( void * pvParameters ) {
   timeClient.begin();
   timeClient.setTimeOffset(25200); // +7 Часовой пояс 

   while(true) {
      if (Zone1.Setting.IsNeedShedule) {
         timeClient.update();
         if(timeClient.getHours() >= 0 || timeClient.getHours() <= 9) {
            Zone1.Setting.IsNightMode = true;
         }

         if(timeClient.getHours() >= 10 || timeClient.getHours() <= 23) {
            Zone1.Setting.IsNightMode = false;
         }
         delay(20000);
      }
   }
}