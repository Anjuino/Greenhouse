#include "TgBot.h"
#include "DeviceGreenhous.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "Tasks/TaskQueue.h"

TgBot* TgBot::Bot = nullptr;

extern class DeviceGreenhous Zone1;
extern class NTPClient timeClient;

uint8_t CurrentMenu = 0;
uint8_t SubMenu = 0;

TgBot::TgBot(class WIFIManagerTgBot *WIFIManagerTgBot) {
    //this->WIFIManager = WIFIManagerTgBot;
    Bot = this;  
}

void TgBot::staticMsgHandler(FB_msg& msg) {
    if (Bot) Bot->newMsg(msg);
}

void TgBot::BotInit() {
    //bot.setToken(WIFIManager->getTokenBot());
    //bot.setChatID(WIFIManager->getUsers());

    bot.setToken("7557142220:AAFDcI7KO0MheGFJO5iuKPwYGhmKKQMnA7g");
    bot.setChatID("942120524");

    bot.setPeriod(1000);

    bot.attach(staticMsgHandler);

    delay(1000);

    ShowGlobalMenu();
}

void TgBot::ShowGlobalMenu()
{
    bot.showMenu("Состояние \n Управление \n Настройки");
}

void TgBot::newMsg(FB_msg& msg)
{
    if (msg.OTA) {
        bot.closeMenu();
        delay(10);
        bot.tickManual();
        bot.update();
    }


    if (msg.text == "Настройки") {
        String Message;

        Message += "Выставленные настройки для мониторинга \n";
        Message += "Сухость почвы max: " + String(Zone1.TypeCrop.GroundWet) + "%\n";
        Message += "Сухость почвы min: " + String(Zone1.TypeCrop.GroundDry) + "%\n";
        Message += "Время полива: "      + String((Zone1.Setting.TimePumpOn / 1000)) + "с\n\n";

        Message += "Влажность воздуха max: " + String(Zone1.TypeCrop.AirWet) + "% \n";
        Message += "Влажность воздуха min: " + String(Zone1.TypeCrop.AirDry) + "% \n";
        Message += "Время увлажнителя: "     + String((Zone1.Setting.TimeHumidifierOn / 1000)) + "с\n\n";

        Message += "Освещение: " + String(Zone1.TypeCrop.Light) + "\n";;
        Message += "Время освещения: "     + String((Zone1.Setting.TimeLampOn/ 1000)) + "с";

        bot.sendMessage (Message);

        if (Zone1.Setting.IsNightMode) bot.showMenu("Выключить ночное расписание\n Мониторинг почвы\n Мониторинг воздуха\n Назад");
        else                           bot.showMenu("Включить ночное расписание \n Мониторинг почвы\n Мониторинг воздуха\n Назад");

        return;
    }

    if (msg.text == "Выключить ночное расписание") {
        Zone1.Setting.IsNightMode = false;
        Zone1.Setting.IsNeedShedule = false;
        EEPROM.put(Zone1.SettingAddress, Zone1.Setting);
        EEPROM.commit();

        bot.sendMessage("Ночное расписание выключено");
        bot.showMenu("Включить ночное расписание \n Мониторинг почвы\n Мониторинг воздуха\n Назад");

        return;
    }

    if (msg.text == "Включить ночное расписание") {
        Zone1.Setting.IsNeedShedule = true;
        EEPROM.put(Zone1.SettingAddress, Zone1.Setting);
        EEPROM.commit();

        bot.sendMessage("Ночное расписание включено");
        bot.showMenu("Выключить ночное расписание \n Мониторинг почвы\n Мониторинг воздуха\n Назад");
        
        return;
    }

    if (msg.text == "Мониторинг почвы") {
        CurrentMenu = 1;
        SubMenu = 1;
        if (Zone1.Setting.WorkModePump == Zone1.Manual) bot.showMenu("Включить автоматический режим\n Назад");
        else                                            bot.showMenu("Включить ручной режим\n Назад");
        return;
    }

    if (msg.text == "Мониторинг воздуха") {
        CurrentMenu = 2;
        SubMenu = 1;
        if (Zone1.Setting.WorkModeHumidifier == Zone1.Manual) bot.showMenu("Включить автоматический режим\n Назад");
        else                                                  bot.showMenu("Включить ручной режим\n Назад");
        return;
    }


    if (msg.text == "Включить автоматический режим") {
        SubMenu = 1;
        String Message;
        if(CurrentMenu == 1) {
            Zone1.Setting.WorkModePump = Zone1.Auto;
            Message += "Мониторинг почвы в автоматическом режиме";
        }
        if(CurrentMenu == 2) {
            Zone1.Setting.WorkModeHumidifier = Zone1.Auto;
            Message += "Мониторинг воздуха в автоматическом режиме";
        }
        EEPROM.put(Zone1.SettingAddress, Zone1.Setting);
        EEPROM.commit();

        bot.sendMessage(Message);
        bot.showMenu("Включить ручной режим\n Назад");
        return;
    }

    if (msg.text == "Включить ручной режим") {
        SubMenu = 1;
        String Message;
        if(CurrentMenu == 1) {
            Zone1.Setting.WorkModePump = Zone1.Manual;
            Message += "Мониторинг почвы в ручном режиме";
        }
        if(CurrentMenu == 2) {
            Zone1.Setting.WorkModeHumidifier = Zone1.Manual;
            Message += "Мониторинг воздуха в ручном режиме";
        }
        EEPROM.put(Zone1.SettingAddress, Zone1.Setting);
        EEPROM.commit();

        bot.sendMessage(Message);
        bot.showMenu("Включить автоматический режим\n Назад");
        return;
    }

    if (msg.text == "Управление") {
        bot.showMenu("Включить полив \n Включить увлажнитель \n Включить освещение \n Назад");
        return;
    }

    if (msg.text == "Включить полив") {
        String Message;
        if (Zone1.IsOnPump) Message = "Полив уже включен";
        else { 
            if(Zone1.PumpOn(Zone1.Setting.TimePumpOn)) {
                Message = "Включил полив";
            }
        }

        if (!Message.isEmpty()) bot.sendMessage (Message);
        return;
    }

    if (msg.text == "Включить увлажнитель") {
        String Message;
        if (Zone1.IsOnHumidifier) Message = "Увлажнитель уже включен";
        else {
            if (Zone1.HumidifierOn(Zone1.Setting.TimeHumidifierOn)) {
                Message = "Включил увлажнитель";
            }
        }

        if (!Message.isEmpty()) bot.sendMessage (Message);
        return;
    }

    if (msg.text == "Включить освещение") {
        String Message;
        if (Zone1.IsOnPump) Message = "Освещение уже включено";
        else {
            Message = "Включил освещение";
            Zone1.LampOn(Zone1.Setting.TimeLampOn);

            JsonDocument doc;
            doc["TypeMessage"] = TypeMessage::StateLamp;
            doc["Message"]     = 1;
            queue.push(doc);
        }

        bot.sendMessage (Message);
        return;
    }

    if (msg.text == "Состояние") {
        String Message;

        // Получаем данные сенсоров
        int16_t Moisture = Zone1.ReadSensor(DeviceGreenhous::TypeSensor::MoistureSensor);
        int16_t Humidity = Zone1.ReadSensor(DeviceGreenhous::TypeSensor::HumiditySensor);
        int16_t IsWater  = Zone1.ReadSensor(DeviceGreenhous::TypeSensor::WaterSensor);

        // Получаем состояние устройств
        String PumpState      = Zone1.IsOnPump ? "Включен" : "Выключен";
        String HumidiferState = Zone1.IsOnHumidifier ? "Включен" : "Выключен";
        String LampState      = Zone1.IsOnLamp ? "Включена" : "Выключена";
        String Water          = IsWater ? "нет" : "есть";

        // Получаем режимы работы
        String MoistureMonitoring = getModeString(Zone1.Setting.WorkModePump);
        String HumidityMonitoring = getModeString(Zone1.Setting.WorkModeHumidifier);
        String LightMonitoring = getModeString(Zone1.Setting.WorkModeLamp);
        String NightMode = Zone1.Setting.IsNightMode ? "Включен" : "Выключен";

        // Получаем информацию о памяти
        uint32_t freeHeap         = ESP.getFreeHeap();
        uint32_t minFreeHeap      = ESP.getMinFreeHeap();
        uint32_t maxAllocHeap     = ESP.getMaxAllocHeap();
        uint8_t heapFragmentation = 100 - (maxAllocHeap * 100 / freeHeap);

        // Формируем сообщение
        Message += "Состояние системы\n\n";
        
        Message += "Параметры среды:\n";
        Message += "• Влажность почвы: " + String(Moisture) + "%\n";
        Message += "• Влажность воздуха: " + String(Humidity) + "%\n\n";
        
        Message += "Состояние устройств:\n";
        Message += "• Полив: " + PumpState + "\n";
        Message += "• Увлажнитель: " + HumidiferState + "\n";
        Message += "• Лампа: " + LampState + "\n";
        Message += "• Наличие воды: " + Water + "\n\n";
        
        Message += "Мониторинг:\n";
        Message += "• Почва: " + MoistureMonitoring + "\n";
        Message += "• Воздух: " + HumidityMonitoring + "\n";
        Message += "• Освещение: " + LightMonitoring + "\n";
        Message += "• Ночной режим: " + NightMode + "\n\n";
        
        Message += "Состояние памяти:\n";
        Message += "• Свободно: " + String(freeHeap/1024) + " KB\n";
        Message += "• Максимальный блок: " + String(maxAllocHeap/1024) + " KB\n";
        Message += "• Фрагментация: " + String(heapFragmentation) + "%\n\n";
        
        Message += "Время на устройстве: " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes());

        bot.sendMessage(Message);
        return;
    }

    if (msg.text == "Меню" || msg.text == "/Start") {
        ShowGlobalMenu();   // Отобразить меню в ТГ боте
        return;
    }

    if (msg.text == "Close") {
        bot.closeMenu();
        return;
    }

    if (msg.text == "Назад") {
        if (SubMenu == 1) {
            SubMenu = 0;
            if (Zone1.Setting.IsNightMode) bot.showMenu("Выключить ночное расписание\n Мониторинг почвы\n Мониторинг воздуха\n Назад");
            else                           bot.showMenu("Включить ночное расписание \n Мониторинг почвы\n Мониторинг воздуха\n Назад");
        }
        else ShowGlobalMenu();   // Отобразить меню в ТГ боте
        return;
    }
}

void TgBot::Reset()
{
    bot.tickManual(); // Чтобы отметить сообщение прочитанным
    delay(10);
    ESP.restart();
}

String TgBot::getModeString(uint8_t mode) {
    switch(mode) {
        case DeviceGreenhous::Mode::Auto: return "Автоматический";
        case DeviceGreenhous::Mode::Manual: return "Ручной";
        case DeviceGreenhous::Mode::Shedule: return "По расписанию";
        default: return "Неизвестно";
    }
}