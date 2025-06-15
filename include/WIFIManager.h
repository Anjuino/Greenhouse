#pragma once
#include "WiFi.h"
#include <WebServer.h>
#include <EEPROM.h>

#include "webPage/index.h"
#include "webPage/css.h"

#define WifiSettingAddress 0                // Адрес настроек для eeprom

class WIFIManager {

    private:

    protected:
        struct{
            char wifiSSID[32];
            char wifiPassword[16];
        } DeviceSetting;


        WebServer server;
        virtual void StartServer();
        void GetCSS();
        void StartPage();
        virtual void RegDeviceData(bool IsNeedRestart = true);
        virtual void Init(bool IsNeedEEPROM);
        virtual void Start(bool IsNeedEEPROM);


    public:

        WIFIManager();
        ~WIFIManager() {};

        void ResetWifiSetting();

        void ScanWifi();
        void GetWifiData();
};