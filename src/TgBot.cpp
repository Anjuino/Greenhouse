#include "TgBot.h"
#include "DeviceGreenhous.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

TgBot* TgBot::Bot = nullptr;

extern class DeviceGreenhous Zone1;
extern class NTPClient timeClient;

uint8_t CurrentMenu = 0;

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

    bot.setToken("8045582620:AAHEhDcspY6rttQ6fja0696QjlFeLPzz8vg");
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
        bot.update();
    }


    if (msg.text == "Настройки") {
        String Message;

        Message += "Выставленные настройки для мониторинга \n";
        Message += "Сухость почвы max: " + String(Zone1.TypeCrop.GroundWet) + "\n";
        Message += "Сухость почвы min: " + String(Zone1.TypeCrop.GroundDry) + "\n";
        Message += "Время полива: "      + String((Zone1.Setting.TimePumpOn / 1000)) + "с\n\n";

        Message += "Влажность воздуха max: " + String(Zone1.TypeCrop.AirWet) + "% \n";
        Message += "Влажность воздуха min: " + String(Zone1.TypeCrop.AirDry) + "% \n";
        Message += "Время увлажнителя: "     + String((Zone1.Setting.TimeHumidifierOn / 1000)) + "с\n\n";

        Message += "Освещение: " + String(Zone1.TypeCrop.Light) + "\n";;
        Message += "Время освещения: "     + String((Zone1.Setting.TimeLampOn/ 1000)) + "с";

        bot.sendMessage (Message);

        if (Zone1.Setting.IsNightMode) bot.showMenu("Выключить ночной режим\n Мониторинг почвы\n Мониторинг воздуха\n Назад");
        else                           bot.showMenu("Включить ночной режим \n Мониторинг почвы\n Мониторинг воздуха\n Назад");

        return;
    }

    if (msg.text == "Выключить ночной режим") {
        Zone1.Setting.IsNightMode = false;
        Zone1.Setting.IsNeedShedule = false;
        EEPROM.put(Zone1.SettingAddress, Zone1.Setting);
        EEPROM.commit();

        bot.sendMessage("Ночной режим выключен");
        return;
    }

    if (msg.text == "Включить ночной режим") {
        //Zone1.Setting.IsNightMode = true;
        Zone1.Setting.IsNeedShedule = true;
        EEPROM.put(Zone1.SettingAddress, Zone1.Setting);
        EEPROM.commit();

        bot.sendMessage("Ночной режим включен");
        return;
    }

    if (msg.text == "Мониторинг почвы") {
        CurrentMenu = 1;
        if (Zone1.Setting.WorkModePump == Zone1.Manual) bot.showMenu("Включить автоматический режим\n Назад");
        else                                            bot.showMenu("Включить ручной режим\n Назад");
        return;
    }

    if (msg.text == "Мониторинг воздуха") {
        CurrentMenu = 2;
        if (Zone1.Setting.WorkModeHumidifier == Zone1.Manual) bot.showMenu("Включить автоматический режим\n Назад");
        else                                                  bot.showMenu("Включить ручной режим\n Назад");
        return;
    }


    if (msg.text == "Включить автоматический режим") {
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
        return;
    }

    if (msg.text == "Включить ручной режим") {
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
        return;
    }

    if (msg.text == "Управление") {
        bot.showMenu("Включить полив \n Включить увлажнитель \n Включить освещение \n Назад");
        return;
    }

    if (msg.text == "Включить полив") {
        String Message;
        if (Zone1.IsOnPump) Message = "Полив уже включен";
        else                Message = "Включил полив";

        Zone1.PumpOn(Zone1.Setting.TimePumpOn);

        bot.sendMessage (Message);
        return;
    }

    if (msg.text == "Включить увлажнитель") {
        String Message;
        if (Zone1.IsOnPump) Message = "Увлажнитель уже включен";
        else                Message = "Включил увлажнитель";

        Zone1.HumidifierOn(Zone1.Setting.TimeHumidifierOn);

        bot.sendMessage (Message);
        return;
    }

    if (msg.text == "Включить освещение") {
        String Message;
        if (Zone1.IsOnPump) Message = "Освещение уже включено";
        else                Message = "Включил освещение";

        Zone1.LampOn(Zone1.Setting.TimeLampOn);

        bot.sendMessage (Message);
        return;
    }

    if (msg.text == "Состояние") {
        String Message;

        int16_t Moisture = Zone1.ReadSensor(DeviceGreenhous::TypeSensor::MoistureSensor);
        int16_t Humidity = Zone1.ReadSensor(DeviceGreenhous::TypeSensor::HumiditySensor);

        String PumpState;
        if (Zone1.IsOnPump) PumpState = "Включен";
        else                PumpState = "Выключен";

        String HumidiferState;
        if (Zone1.IsOnHumidifier) HumidiferState = "Включен";
        else                      HumidiferState = "Выключен";

        String LampState;
        if (Zone1.IsOnLamp) LampState = "Включена";
        else                LampState = "Выключена";

        String MoistureMonitoring;
        if (Zone1.Setting.WorkModePump == DeviceGreenhous::Mode::Auto)    MoistureMonitoring = "Автоматический";
        if (Zone1.Setting.WorkModePump == DeviceGreenhous::Mode::Manual)  MoistureMonitoring = "Ручной";
        if (Zone1.Setting.WorkModePump == DeviceGreenhous::Mode::Shedule) MoistureMonitoring = "По расписанию";

        String HumidityMonitoring;
        if (Zone1.Setting.WorkModeHumidifier == DeviceGreenhous::Mode::Auto)    HumidityMonitoring = "Автоматический";
        if (Zone1.Setting.WorkModeHumidifier == DeviceGreenhous::Mode::Manual)  HumidityMonitoring = "Ручной";
        if (Zone1.Setting.WorkModeHumidifier == DeviceGreenhous::Mode::Shedule) HumidityMonitoring = "По расписанию";

        String LightMonitoring;
        if (Zone1.Setting.WorkModeLamp == DeviceGreenhous::Mode::Auto)    LightMonitoring = "Автоматический";
        if (Zone1.Setting.WorkModeLamp == DeviceGreenhous::Mode::Manual)  LightMonitoring = "Ручной";
        if (Zone1.Setting.WorkModeLamp == DeviceGreenhous::Mode::Shedule) LightMonitoring = "По расписанию";
        
        String NightMode;
        if (Zone1.Setting.IsNightMode) NightMode = "Включен";
        else                           NightMode = "Выключен";

        String TimeMode;
        if (Zone1.Setting.IsNeedShedule) TimeMode = "Включено";
        else                             TimeMode = "Выключено";           

        Message += "Состояние \n";

        Message += "Влажность почвы: " + String(Moisture) + "\n";
        Message += "Влажность воздуха: " + String(Humidity) + "\n";
        Message += "Освещенность: " + String(100) + " \n\n";

        Message += "Полив: " + PumpState + "\n";
        Message += "Увлажнитель: " + HumidiferState + "\n";
        Message += "Лампа: " + LampState + " \n\n";

        Message += "Мониторинг: \n";
        Message += "Почва: " + MoistureMonitoring  + "\n";
        Message += "Воздух: " + HumidityMonitoring +  "\n";
        Message += "Освещение: " + LightMonitoring + "\n";
        Message += "Ночной режим: " + LightMonitoring + "\n";
        Message += "Расписание: " + TimeMode + "\n";
        Message += "Время на устройстве: " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + "\n";

        bot.sendMessage (Message);

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
        ShowGlobalMenu();   // Отобразить меню в ТГ боте
        return;
    }
}

void TgBot::Reset()
{
    bot.tickManual(); // Чтобы отметить сообщение прочитанным
    delay(10);
    ESP.restart();
}