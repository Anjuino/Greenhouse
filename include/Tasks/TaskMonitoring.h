#include "DeviceGreenhous.h"
#include "DataTable.h"
class DeviceGreenhous Zone1(Mint, 260);

TaskHandle_t Monitoring_taskZone1;
void MonitoringCodeZone1(void * pvParameters) {
  
  Zone1.Init(23,22,36,39,1);
  //Serial.println("Сухость почвы в настройках");
  //Serial.println(Zone1.TypeCrop.GroundDry);
  while(true) {
    Zone1.MonitoringMoisture();
    Zone1.MonitoringHumidity();
    Zone1.MonitoringLighting();
    delay(1);
  }
}