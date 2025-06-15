#include "TgBot.h"

TgBot* TgBot::Bot = nullptr;

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
        String Messeage = "Состояние";

        bot.sendMessage (Messeage);

        return;
    }

    if (msg.text == "Меню") {
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