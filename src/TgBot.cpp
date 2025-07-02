#include "TgBot.h"
#include "DeviceGreenhous.h"

TgBot* TgBot::Bot = nullptr;

extern class DeviceGreenhous Zone1;

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

}

void TgBot::newMsg(FB_msg& msg)
{
    if (msg.OTA) {
        bot.closeMenu();
        delay(10);
        bot.update();
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
        

        Message += "Состояние \n\n";

        Message += "Влажность земли: " + String(Moisture) + "\n\n";
        Message += "Влажность воздуха: " + String(Humidity) + " % \n\n";
        Message += "Освещенность: " + String(100) + " \n\n";
        
        Message += "Полив: " + PumpState + " \n\n";
        Message += "Увлажнитель: " + HumidiferState + " \n\n";
        Message += "Лампа: " + LampState + " \n\n";

        Message += "Мониторинг: \n\n";
        Message += "---Земля: " + MoistureMonitoring;
        Message += "---Воздух: " + HumidityMonitoring;
        Message += "---Освещения: " + LightMonitoring;

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

    /*if (msg.text == "Настройки") {
        bot.showMenu("Сброс Wifi \n Сброс всех настроек \n Назад");
        return;
    }*/
    

    if (msg.text == "Назад") {
        ShowGlobalMenu();   // Отобразить меню в ТГ боте
        return;
    }

    /*if (msg.text == "Сброс Wifi") {
        bot.sendMessage ("Сбрасываю настройки wifi");
        WIFIManager->ResetWifiSetting();
        bot.sendMessage ("Перезагрузка");
        Reset();
        return;
    }

    if (msg.text == "Сброс всех настроек") {
        bot.sendMessage ("Сбрасываю все настройки");
        WIFIManager->ResetWifiSetting();
        WIFIManager->ResetBotSettings();
        bot.sendMessage ("Перезагрузка");
        Reset();
        return;
    }*/
}

void TgBot::Reset()
{
    bot.tickManual(); // Чтобы отметить сообщение прочитанным
    delay(10);
    ESP.restart();
}