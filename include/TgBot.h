#pragma once
#include <FastBot.h>
#include "WIFIManagerTgBot.h"

class TgBot {
    
    private:
        FastBot bot;

        //WIFIManagerTgBot* WIFIManager;
        static TgBot* Bot; 
        
        void newMsg(FB_msg& msg);
        void ShowGlobalMenu();
        void Reset();
        String getModeString(uint8_t mode);

    public:
        TgBot(class WIFIManagerTgBot *WIFIManager);
        ~TgBot() {};    

        void BotInit();
        void Loop() {bot.tick();};

        void SendMessage(String Message) {bot.sendMessage(Message);};

        static void staticMsgHandler(FB_msg& msg);
};

