#include "WIFIManagerTgBot.h"


WIFIManagerTgBot::WIFIManagerTgBot()
{

}

void WIFIManagerTgBot::Init(bool IsNeedEEPROM)
{
    WIFIManager::Init(IsNeedEEPROM);    // Чтение настроек WIFI из eeprom

    EEPROM.get (TelegramBotSettingAddress, TgBotSetting); // Чтение настроек ТГ бота из eeprom

    bool TokenBotIsEmpty = false;
    uint8_t count;
    for (int i = 0; i < sizeof(TgBotSetting.TokenBot) / 2; i++) {
        if (TgBotSetting.TokenBot[i] != (char)0xFF) {
            count++;
            if (count == 5) {
                TokenBotIsEmpty = true;
                break;
            }
        }
    }

    if (!TokenBotIsEmpty) ResetBotSettings();

    Serial.println(TgBotSetting.TokenBot);
    Serial.println(TgBotSetting.Users);
}

void WIFIManagerTgBot::Start(bool IsNeedEEPROM)
{
    WIFIManager::Start(IsNeedEEPROM);   // Базовое подключение к WIFI сети

    if (strlen(TgBotSetting.TokenBot) != 0 || strlen(TgBotSetting.Users) != 0) return;
    else {                         // Если подключение получилось, а настроек для ТГ бота нет, то создаю заново страницу для ввода данных
        digitalWrite(16, LOW);
        digitalWrite(26, HIGH);

        Serial.println("Запускаю точку доступа");
        WiFi.softAP("ESP32", "password");

        StartServer();
        
        while(true) {
            server.handleClient ();
            delay(0);
        }
    }
}

void WIFIManagerTgBot::RegDeviceData(bool IsNeedRestart)
{
    WIFIManager::RegDeviceData(false);

    String BotToken = server.arg ("Token");
    String Users = server.arg ("Users");

    Serial.println(BotToken);
    Serial.println(Users);

    if (BotToken.length() < sizeof(TgBotSetting.TokenBot) && Users.length() < sizeof(TgBotSetting.Users)) {
        if (BotToken.length() != 0) BotToken.toCharArray(TgBotSetting.TokenBot, sizeof(TgBotSetting.TokenBot));
        if (Users.length() != 0)    Users.toCharArray(TgBotSetting.Users, sizeof(TgBotSetting.Users));

        EEPROM.put (TelegramBotSettingAddress, TgBotSetting);
        EEPROM.commit();
        delay(100);
    
        Serial.println(TgBotSetting.TokenBot);
        Serial.println(TgBotSetting.Users);
        if (IsNeedRestart) ESP.restart();
    } else server.send (503, "text/plane", "Fail");
}

void WIFIManagerTgBot::ResetBotSettings()
{
    for (int i = 0; i < sizeof(TgBotSetting); i++) { 
        EEPROM.write((TelegramBotSettingAddress + i), 0x00);
    }
    EEPROM.commit();
}


void WIFIManagerTgBot::UpdateBotUsers(String Users)
{
    Users.toCharArray(TgBotSetting.Users, sizeof(TgBotSetting.Users));

    EEPROM.put (TelegramBotSettingAddress, TgBotSetting);
    EEPROM.commit();
}

void WIFIManagerTgBot::GetTgBotData()
{
    String JSONAnswer = "{";
    JSONAnswer += "\"BotToken\":\"" + String(TgBotSetting.TokenBot) + "\" ,";
    JSONAnswer += "\"Users\":\"" + String(TgBotSetting.Users) + "\"}";

    server.send(200, "application/json", JSONAnswer);
}

void WIFIManagerTgBot::StartServer()
{
    WIFIManager::StartServer();

    server.on("/GetTgBotData", [this]() {GetTgBotData();});      // Запрос на получение данных о сохраненной сети
}
