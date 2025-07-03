#include "DeviceGreenhous.h"
#include "EEPROM.h"

DeviceGreenhous::DeviceGreenhous(DataCrop TypeCrop, uint16_t SettingAddress)
{
    this->TypeCrop = TypeCrop;
    this->SettingAddress = SettingAddress;
}

DeviceGreenhous::~DeviceGreenhous()
{
    if (dht) delete dht;
}

void DeviceGreenhous::Init(uint8_t Port_Humidifier, uint8_t Port_Pump, uint8_t Port_MoistureSensor, uint8_t Port_LightSensor, uint8_t Port_HumidiferSensor)
{
    PhysicsPin.Port_Humidifier     = Port_Humidifier;
    PhysicsPin.Port_Pump           = Port_Pump;
    PhysicsPin.Port_MoistureSensor = Port_MoistureSensor;
    PhysicsPin.Port_LightSensor    = Port_LightSensor;

    pinMode(PhysicsPin.Port_Humidifier, OUTPUT);        // Порт увлажнителя в режиме выхода
    pinMode(PhysicsPin.Port_Pump, OUTPUT);              // Порт насоса в режиме выхода

    digitalWrite(PhysicsPin.Port_Humidifier, HIGH);      // При старте пишу 1. Выключено по умолчанию
    digitalWrite(PhysicsPin.Port_Pump, HIGH);

    Lamp.setPin(PhysicsPin.Port_LightSensor);
    Lamp.updateLength(196);
    Lamp.updateType(NEO_GRB + NEO_KHZ800);

    EEPROM.get(SettingAddress, Setting);

    if (Setting.SettingIsEmpty) {
        Setting.SettingIsEmpty = false;

        Setting.WorkModePump = 1;
        Setting.TimePumpOn   = 5000;

        Setting.WorkModeHumidifier = 1;
        Setting.TimeHumidifierOn   = 120000;

        Setting.WorkModeLamp = 0;
        Setting.TimeLampOn = 1800000;

        EEPROM.put(SettingAddress, Setting);
        EEPROM.commit();
    }
    if (!dht) {
        dht = new DHT(Port_HumidiferSensor, DHT11);
        dht->begin();
    }
}

int16_t DeviceGreenhous::ReadSensor(uint8_t TypeSensor)
{
    if (TypeSensor == MoistureSensor) {
    
        for (uint8_t Count = 0; Count < 3; Count++) {
            Moisture += analogRead(PhysicsPin.Port_MoistureSensor);
        }

        Moisture = Moisture / 3;
        
        return Moisture;
    }

    if (TypeSensor == HumiditySensor) {
        return dht->readHumidity();
    }

    if (TypeSensor == LightSensor) {
    }

    return 0;
}

void DeviceGreenhous::PumpOn(uint16_t Timer)
{
    TimerPump = millis() + Timer;
    IsOnPump = true;
    digitalWrite(PhysicsPin.Port_Pump, LOW);
    Serial.println("Включаю помпу"); 
}

void DeviceGreenhous::LampOn(uint16_t Timer)
{
    TimerLamp = millis() + Timer;
    IsOnLamp = true;

    for (uint16_t i = 0; i < Lamp.numPixels (); i++) {
        Lamp.setPixelColor (i, 255, 0, 255);
    }
    Lamp.show ();
}

void DeviceGreenhous::HumidifierOn(uint16_t Timer)
{
    TimerHumidifier = millis() + Timer;
    IsOnHumidifier = true;
    digitalWrite(PhysicsPin.Port_Humidifier, HIGH); 
}

void DeviceGreenhous::CheckTimerHumidifier()
{
    if(TimerHumidifier < millis() && IsOnHumidifier) {
        digitalWrite(PhysicsPin.Port_Humidifier, LOW);
        IsOnHumidifier = false;
    } 
}

void DeviceGreenhous::CheckTimerPump()
{
    if(TimerPump < millis() && IsOnPump) {
        digitalWrite(PhysicsPin.Port_Pump, HIGH);
        IsOnPump = false;
        Serial.println("Выключаю помпу"); 
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
    } 
}

void DeviceGreenhous::MonitoringMoisture()
{
    CheckTimerPump();
    
    if(Setting.WorkModePump == Auto) {
        if(millis() > TimerMonitoringPump) {
            TimerMonitoringPump = millis() + 2000;

            ReadSensor(MoistureSensor);
            Serial.println(Moisture); 

            if(Moisture > TypeCrop.GroundDry) {
                TimerMonitoringPump = millis() + 20000;
                PumpOn(Setting.TimePumpOn);
            }
        }
    }
}

void DeviceGreenhous::MonitoringHumidity()
{
    CheckTimerHumidifier();
    
    if(Setting.WorkModeHumidifier == Auto) {
        if(millis() > TimerMonitoringHumidifier) {
            TimerMonitoringHumidifier = millis() + 120000;
            ReadSensor(HumiditySensor);
            
            if (Humidity < TypeCrop.AirDry) {
                HumidifierOn(Setting.TimeHumidifierOn);
                TimerMonitoringHumidifier = millis() + 20000;
            }
        }
    }
}

void DeviceGreenhous::MonitoringLighting()
{
    CheckTimerLighiting();

    if(Setting.WorkModeLamp == Auto) {
        if(millis() > TimerMonitoringLamp) {
            TimerMonitoringLamp = millis() + 120000;
            ReadSensor(LightSensor);
        }
    }
}