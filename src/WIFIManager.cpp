#include "WIFIManager.h"

WIFIManager::WIFIManager()
{   

}

void WIFIManager::Init(bool IsNeedEEPROM)
{
    if (IsNeedEEPROM) EEPROM.begin (512);

    delay(10);
    
    EEPROM.get (WifiSettingAddress, DeviceSetting);

    bool IsSSID = false;
    uint8_t count;
    for (int i = 0; i < sizeof(DeviceSetting.wifiSSID) / 2; i++) {
        if (DeviceSetting.wifiSSID[i] != (char)0xFF) {
            count++;
            if (count == 5) {
                IsSSID = true;
                break;
            }
        }
    }

    count = 0;

    bool IsPassword = false;
    for (int i = 0; i < sizeof(DeviceSetting.wifiPassword) / 2; i++) {
        if (DeviceSetting.wifiPassword[i] != (char)0xFF) {
            count++;
            if (count == 5) {
                IsPassword = true;
                break;
            }
        }
    }

    if (!IsSSID || !IsPassword) ResetWifiSetting();

    Serial.println(DeviceSetting.wifiSSID);
    Serial.println(DeviceSetting.wifiPassword);
}

void WIFIManager::ResetWifiSetting()
{
    for (int i = 0; i < sizeof(DeviceSetting); i++) { 
        EEPROM.write(i, 0x00);
    }
    EEPROM.commit();
}

void WIFIManager::Start(bool IsNeedEEPROM)
{   
    Init(IsNeedEEPROM);

    WiFi.mode(WIFI_STA);     // Включаю режим станиции

    WiFi.begin(DeviceSetting.wifiSSID, DeviceSetting.wifiPassword);   // Попытка подключения к сети

    uint64_t TimeConnect = millis() + 6000;
    bool IsConnect = true;
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
      if (millis() > TimeConnect) {
        IsConnect = false;
        break;
      }
    }
                                                                                      
    // Не смог подключится к сети. Создаю точку доступа и запускаю сервер
    if (!IsConnect) {
        Serial.println("Запускаю точку доступа");
        WiFi.softAP("ESP", "password");

        StartServer();
        
        while(true) {
            server.handleClient ();
            delay(0);
        }
    }
}


void WIFIManager::GetCSS()
{
    server.send (200, "text/css", css);
}

void WIFIManager::StartPage()
{
    server.send (200, "text/html", Index);
}

void WIFIManager::GetWifiData()
{
    String JSONAnswer = "{";
    JSONAnswer += "\"SSID\":\"" + String(DeviceSetting.wifiSSID) + "\" ,";
    JSONAnswer += "\"Password\":\"" + String(DeviceSetting.wifiPassword) + "\"}";

    server.send(200, "application/json", JSONAnswer);
}

void WIFIManager::ScanWifi()
{
    int netCount = WiFi.scanNetworks();
    String JSONAnswer = "["; 

    for (int i = 0; i < netCount; i++) {
        JSONAnswer += "{"; 

        JSONAnswer += "\"SSID\":\"" + String(WiFi.SSID(i)) + "\",";
        JSONAnswer += "\"RSSI\":\"" + String(WiFi.RSSI(i)) + "\"";

        JSONAnswer += "}"; 

        if (i < netCount - 1) JSONAnswer += ",";
    }

    JSONAnswer += "]"; 

    server.send(200, "application/json", JSONAnswer);
}

void WIFIManager::RegDeviceData(bool IsNeedRestart) 
{
  String SSID = server.arg ("SSID");
  String Password = server.arg ("Password");

  if (SSID.length() < sizeof(DeviceSetting.wifiSSID) && Password.length() < sizeof(DeviceSetting.wifiPassword)) {
    if (SSID.length() != 0)      SSID.toCharArray(DeviceSetting.wifiSSID, sizeof(DeviceSetting.wifiSSID));
    if (Password.length() != 0)  Password.toCharArray(DeviceSetting.wifiPassword, sizeof(DeviceSetting.wifiPassword));

    EEPROM.put (WifiSettingAddress, DeviceSetting);
    EEPROM.commit();
    Serial.println(DeviceSetting.wifiSSID);
    Serial.println(DeviceSetting.wifiPassword);

    if (IsNeedRestart) ESP.restart();
  } else server.send (503, "text/plane", "Fail");

  server.send (200, "text/plane", "OK");
}

void WIFIManager::StartServer()
{
    server.on("/styles.css", [this]() {GetCSS();});
    server.on("/", [this]() {StartPage();});
    server.on("/RegDeviceData", [this]() {RegDeviceData();});  // Обработчик входящих данных
    server.on("/ScanWifi", [this]() {ScanWifi();});            // Сканирование WIFI

    server.on("/GetWifiData", [this]() {GetWifiData();});      // Запрос на получение данных о сохраненной сети

    server.begin();
}