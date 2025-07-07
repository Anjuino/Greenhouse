#include "DeviceGreenhous.h"
#include "DataTable.h"
class DeviceGreenhous Zone1(Mint, 260);

TaskHandle_t Monitoring_taskZone1;
void MonitoringCodeZone1(void * pvParameters) {
  Serial.println("Задача по мониторингу");
  Zone1.Init(23,22,39,36,21);
  uint64_t Time = millis() + 20000;
  while(true) {
    Zone1.MonitoringMoisture();
    Zone1.MonitoringHumidity();
    Zone1.MonitoringLighting();
    delay(1);

    if (Time < millis()) {
      Time = millis() + 60000 * 20;

      JsonDocument doc;
      doc["TypeMessage"] = TypeMessage::GroundHumidity;
      doc["Message"]     = Zone1.ReadSensor(DeviceGreenhous::MoistureSensor);
      queue.push(doc);
      
      doc["TypeMessage"] = TypeMessage::AirHumidity;
      doc["Message"]     = Zone1.ReadSensor(DeviceGreenhous::HumiditySensor);
      queue.push(doc); 
    }

  }
}