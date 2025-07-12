#pragma once
#include <FastBot.h>
#include "WIFIManagerTgBot.h"

class TgBot {
    
    private:
        FastBot bot;
        static TgBot* Bot; 
        
        void newMsg(FB_msg& msg);
        void ShowGlobalMenu();
        void Reset();
        String getModeString(uint8_t mode);

    public:
        TgBot(class WIFIManagerTgBot *WIFIManager);
        TgBot() {Bot = this;};
        ~TgBot() {};    

        void BotInit();
        void Loop() {bot.tick();};

        void SendMessage(String Message) {bot.sendMessage(Message);};

        static void staticMsgHandler(FB_msg& msg);
};

