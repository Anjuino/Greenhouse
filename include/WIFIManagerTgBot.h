#pragma once
#include "WIFIManager.h"
#define TelegramBotSettingAddress 50       // Адрес настроек для eeprom


class WIFIManagerTgBot:public WIFIManager {

    private:

        struct{
            char TokenBot[80];
            char Users[120];
        } TgBotSetting;

        void GetTgBotData();
        
    public:

        WIFIManagerTgBot();
        ~WIFIManagerTgBot() {};

        void Init(bool IsNeedEEPROM) override;
        void Start(bool IsNeedEEPROM) override;

        void StartServer() override;

        void RegDeviceData(bool IsNeedRestart = true) override;
        void ResetBotSettings();

        const char* getTokenBot() const {return TgBotSetting.TokenBot;}
        const char* getUsers() const {return TgBotSetting.Users;}

        void UpdateBotUsers(String Users);
};