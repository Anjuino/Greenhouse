/*
Контакты ADC2 нельзя использовать, когда включен Wi-Fi.
ADC1 - GPIO36, 39, 34, 35, 32, 33
*/

#pragma once
#include "Arduino.h"
#include "DataTable.h"
#include <Adafruit_NeoPixel.h>
#define DeviceSettings 260  

class DeviceGreenhous {

    private:

        Adafruit_NeoPixel Lamp;

        uint64_t TimerMonitoringPump = 0;           // Время монитронига для полива
        uint64_t TimerMonitoringHumidifier = 0;     // Время монитронига для влажности воздуха
        uint64_t TimerMonitoringLamp = 0;           // Время монитронига для освещения
        
        enum Mode {
            Manual  = 0,
            Auto    = 1,
            Shedule = 2
        };

        enum TypeSensor {
            MoistureSensor        = 1,
            HumiditySensor        = 2,
            TemperatureSensor     = 3,
            LightSensor           = 4
        };

        struct {
            uint8_t Port_Humidifier;
            uint8_t Port_Pump;
            uint8_t Port_MoistureSensor;
            uint8_t Port_LightSensor;
        } PhysicsPin;
        
        struct {
            bool SettingIsEmpty;
            uint8_t WorkModePump = 1;               // Режим работы мониторнига 0 - ручной, 1 - автоматический, 2 - по расписанию
            uint64_t TimePumpOn  = 5000;            // Продолжительность работы
            
            uint8_t WorkModeHumidifier = 0;
            uint64_t TimeHumidifierOn  = 30000;
              
            uint8_t WorkModeLamp = 0;
            uint64_t TimeLampOn  = 1800000;
             
        } Setting;

        uint64_t TimerPump = 0;         // Время работы
        uint64_t TimerHumidifier = 0;
        uint64_t TimerLamp = 0;

        bool IsOnPump = false;          // Работа насоса
        bool IsOnHumidifier = false;    // Работа увлажнителя
        bool IsOnLamp = false;          // Работа освещения

        int16_t Moisture = 0;           // Влажность почвы
        int16_t Humidity = 0;           // Влажность воздуха
        int16_t Light    = 0;           // Освещенность зоны 
        

        void CheckTimerPump();
        void CheckTimerHumidifier();
        void CheckTimerLighiting();

    public:
        DataCrop TypeCrop;

        DeviceGreenhous(DataCrop TypeCrop);
        ~DeviceGreenhous();

        void Init(uint8_t Port_Humidifier, uint8_t Port_Pump, uint8_t Port_MoistureSensor, uint8_t Port_LightSensor);
        
        void MonitoringMoisture();
        void MonitoringHumidity();
        void MonitoringLighting();

        void PumpOn(uint16_t Timer);
        void HumidifierOn(uint16_t Timer);
        void LampOn(uint16_t Timer);

        int16_t ReadSensor(uint8_t TypeSensor);
};