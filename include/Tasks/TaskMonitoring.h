#include "DeviceGreenhous.h"
#include "DataTable.h"
class DeviceGreenhous Zone1(Mint, 260);

TaskHandle_t Monitoring_taskZone1;
void MonitoringCodeZone1(void * pvParameters) {
  Zone1.Init(23,22,39,36,21,19);
  uint64_t Time = millis() + 20000;
  while(true) {
    Zone1.MonitoringMoisture();
    Zone1.MonitoringHumidity();
    Zone1.MonitoringLighting();
    delay(1);

    if (Time < millis()) {
      Time = millis() + 60000 * 20;

      JsonDocument *doc1 = new JsonDocument();
      (*doc1)["TypeMessage"] = TypeMessage::GroundHumidity;
      (*doc1)["Message"]     = Zone1.ReadSensor(DeviceGreenhous::MoistureSensor);
      xQueueSend(queue, &doc1, portMAX_DELAY); 
      //queue.push(doc);
      
      JsonDocument *doc2 = new JsonDocument();
      (*doc2)["TypeMessage"] = TypeMessage::AirHumidity;
      (*doc2)["Message"]     = Zone1.ReadSensor(DeviceGreenhous::HumiditySensor);
      xQueueSend(queue, &doc2, portMAX_DELAY); 
      //queue.push(doc); 

      JsonDocument *doc3 = new JsonDocument();
      (*doc3)["TypeMessage"] = TypeMessage::StatePump;
      (*doc3)["Message"]     = Zone1.IsOnPump ? 1 : 0;
      xQueueSend(queue, &doc3, portMAX_DELAY); 
      //queue.push(doc);
      
      JsonDocument *doc4 = new JsonDocument();
      (*doc4)["TypeMessage"] = TypeMessage::StateHumidifer;
      (*doc4)["Message"]     = Zone1.IsOnHumidifier ? 1 : 0;
      xQueueSend(queue, &doc4, portMAX_DELAY); 
      //queue.push(doc); 
    }
  }
}