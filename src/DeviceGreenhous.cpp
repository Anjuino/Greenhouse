#include "DeviceGreenhous.h"
#include "EEPROM.h"
#include "Tasks/TaskQueue.h"
#include "freertos/queue.h"

DeviceGreenhous::DeviceGreenhous(DataCrop TypeCrop, uint16_t SettingAddress)
{
    this->TypeCrop = TypeCrop;
    this->SettingAddress = SettingAddress;
}

DeviceGreenhous::~DeviceGreenhous()
{
    if (dht) delete dht;
}

void DeviceGreenhous::Init(uint8_t Port_Humidifier, uint8_t Port_Pump, uint8_t Port_MoistureSensor, uint8_t Port_LightSensor, uint8_t Port_HumiditySensor, uint8_t Port_WaterSensor)
{
    PhysicsPin.Port_Humidifier     = Port_Humidifier;
    PhysicsPin.Port_Pump           = Port_Pump;
    PhysicsPin.Port_MoistureSensor = Port_MoistureSensor;
    PhysicsPin.Port_LightSensor    = Port_LightSensor;
    PhysicsPin.Port_HumiditySensor = Port_HumiditySensor;
    PhysicsPin.Port_WaterSensor    = Port_WaterSensor;

    pinMode(PhysicsPin.Port_Humidifier, OUTPUT);        // Порт увлажнителя в режиме выхода
    pinMode(PhysicsPin.Port_Pump, OUTPUT);              // Порт насоса в режиме выхода
    pinMode(PhysicsPin.Port_WaterSensor, INPUT);  // Начинаем в отключенном состоянии
 
    digitalWrite(PhysicsPin.Port_Humidifier, HIGH);      // При старте пишу 1. Выключено по умолчанию
    digitalWrite(PhysicsPin.Port_Pump, HIGH);

    Lamp.setPin(PhysicsPin.Port_LightSensor);
    Lamp.updateLength(196);
    Lamp.updateType(NEO_GRB + NEO_KHZ800);
    Lamp.setBrightness(200);

    EEPROM.get(SettingAddress, Setting);

    if (Setting.SettingIsEmpty) {
        Setting.SettingIsEmpty = false;

        Setting.WorkModePump = 1;
        Setting.TimePumpOn   = 7000;

        Setting.WorkModeHumidifier = 1;
        Setting.TimeHumidifierOn   = 240000;

        Setting.WorkModeLamp = 0;
        Setting.TimeLampOn = 60000 * 60;

        Setting.IsNeedShedule = true;
        Setting.IsNightMode   = false;

        EEPROM.put(SettingAddress, Setting);
        EEPROM.commit();
    }
    if (!dht) {
        dht = new DHT(PhysicsPin.Port_HumiditySensor, DHT22);
        dht->begin();
    }
}

int16_t DeviceGreenhous::ReadSensor(uint8_t TypeSensor)
{
    if (TypeSensor == MoistureSensor) {
        
        int rawMoisture = analogRead(PhysicsPin.Port_MoistureSensor);
        filteredMoisture = alpha * rawMoisture + (1 - alpha) * filteredMoisture;

        Moisture = map(filteredMoisture, 2700, 1100, 0, 100);
        Moisture = constrain(Moisture, 0, 100);
        return Moisture;
    }

    if (TypeSensor == HumiditySensor) {
        Humidity = dht->readHumidity();
        return Humidity;
    }

    if (TypeSensor == LightSensor) {
    }

    if (TypeSensor == WaterSensor) {
        pinMode(PhysicsPin.Port_WaterSensor, OUTPUT);
        digitalWrite(PhysicsPin.Port_WaterSensor, HIGH);

        delay(40);  
        
        pinMode(PhysicsPin.Port_WaterSensor, INPUT_PULLUP);
        delay(40);  
        
        int16_t waterStatus = digitalRead(PhysicsPin.Port_WaterSensor);
        
        pinMode(PhysicsPin.Port_WaterSensor, INPUT);
        
        return waterStatus;
    }

    return 0;
}

bool DeviceGreenhous::PumpOn(uint64_t Timer)
{   
    JsonDocument *doc = new JsonDocument();
    (*doc)["TypeMessage"] = TypeMessage::GroundHumidity;
    (*doc)["Message"]     = Moisture;
    xQueueSend(queue, &doc, portMAX_DELAY); 
    //queue.push(doc);

    int16_t IsWater = ReadSensor(WaterSensor);

    if(!IsWater) {
        TimerPump = millis() + Timer;
        IsOnPump = true;
        digitalWrite(PhysicsPin.Port_Pump, LOW);

        JsonDocument *doc = new JsonDocument();
        (*doc)["TypeMessage"] = TypeMessage::StatePump;
        (*doc)["Message"]     = 1;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);
        return true;

    } else {
        (*doc)["TypeMessage"] = TypeMessage::StateWater;
        (*doc)["Message"]     = 0;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);
    }
    return false;
}

void DeviceGreenhous::LampOn(uint64_t Timer)
{
    TimerLamp = millis() + Timer;
    IsOnLamp = true;

    for (uint16_t i = 0; i < Lamp.numPixels (); i++) {
        Lamp.setPixelColor (i, 255, 0, 255);
    }
    Lamp.show ();

    JsonDocument *doc = new JsonDocument();
    (*doc)["TypeMessage"] = TypeMessage::StateLamp;
    (*doc)["Message"]     = 1;
    xQueueSend(queue, &doc, portMAX_DELAY); 
    //queue.push(doc);
}

bool DeviceGreenhous::HumidifierOn(uint64_t Timer)
{   
    JsonDocument *doc = new JsonDocument();
    (*doc)["TypeMessage"] = TypeMessage::AirHumidity;
    (*doc)["Message"]     = Humidity;
    xQueueSend(queue, &doc, portMAX_DELAY); 
    //queue.push(doc);

    int16_t IsWater = ReadSensor(WaterSensor);

    if(!IsWater) {
        TimerHumidifier = millis() + Timer;
        IsOnHumidifier = true;
        digitalWrite(PhysicsPin.Port_Humidifier, LOW);

        (*doc)["TypeMessage"] = TypeMessage::StateHumidifer;
        (*doc)["Message"]     = 1;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);

        return true;
    } else {
        (*doc)["TypeMessage"] = TypeMessage::StateWater;
        (*doc)["Message"]     = 0;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);
    }

    return false;
}

void DeviceGreenhous::CheckTimerHumidifier()
{
    if(TimerHumidifier < millis() && IsOnHumidifier) {
        digitalWrite(PhysicsPin.Port_Humidifier, HIGH);
        IsOnHumidifier = false;

        JsonDocument *doc = new JsonDocument();
        (*doc)["TypeMessage"] = TypeMessage::StateHumidifer;
        (*doc)["Message"]     = 0;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);

        (*doc)["TypeMessage"] = TypeMessage::AirHumidity;
        (*doc)["Message"]     = ReadSensor(DeviceGreenhous::MoistureSensor);
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);
    } 
}

void DeviceGreenhous::CheckTimerPump()
{
    if(TimerPump < millis() && IsOnPump) {
        digitalWrite(PhysicsPin.Port_Pump, HIGH);
        IsOnPump = false;

        JsonDocument *doc = new JsonDocument();
        (*doc)["TypeMessage"] = TypeMessage::StatePump;
        (*doc)["Message"]     = 0;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc); 

        (*doc)["TypeMessage"] = TypeMessage::GroundHumidity;
        (*doc)["Message"]     = ReadSensor(DeviceGreenhous::MoistureSensor);
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc);
    }
}

void DeviceGreenhous::CheckTimerLighiting()
{
    if(TimerLamp < millis() && IsOnLamp) {
        IsOnLamp = false;
        for (uint16_t i = 0; i < Lamp.numPixels (); i++) {
            Lamp.setPixelColor (i, 0, 0, 0);
        }
        Lamp.show ();

        JsonDocument *doc = new JsonDocument();
        (*doc)["TypeMessage"] =TypeMessage::StateLamp;
        (*doc)["Message"]     = 0;
        xQueueSend(queue, &doc, portMAX_DELAY); 
        //queue.push(doc); 
    } 
}

void DeviceGreenhous::MonitoringMoisture()
{
    CheckTimerPump();
    
    if (!Setting.IsNightMode) {
        if(Setting.WorkModePump == Auto) {
            if(millis() > TimerMonitoringPump) {
                TimerMonitoringPump = millis() + 60000*20;

                ReadSensor(MoistureSensor);
                if(Moisture < TypeCrop.GroundDry) {
                    PumpOn(Setting.TimePumpOn);
                }
            }
        }
    }
}

void DeviceGreenhous::MonitoringHumidity()
{
    CheckTimerHumidifier();
    
    if (!Setting.IsNightMode) {
        if(Setting.WorkModeHumidifier == Auto) {
            if(millis() > TimerMonitoringHumidifier) {
                TimerMonitoringHumidifier = millis() + 60000*20;
                ReadSensor(HumiditySensor);
                
                if (Humidity < TypeCrop.AirDry) {
                    HumidifierOn(Setting.TimeHumidifierOn);
                }
            }
        }
    }
}

void DeviceGreenhous::MonitoringLighting()
{
    CheckTimerLighiting();

    if (!Setting.IsNightMode) {
        if(Setting.WorkModeLamp == Auto) {
            if(millis() > TimerMonitoringLamp) {
                TimerMonitoringLamp = millis() + 60000*20;
                ReadSensor(LightSensor);
            }
        }
    }
}