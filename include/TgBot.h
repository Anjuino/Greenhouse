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

    public:
        TgBot(class WIFIManagerTgBot *WIFIManager);
        ~TgBot() {};    

        void BotInit();
        void Loop() {bot.tick();};

        static void staticMsgHandler(FB_msg& msg);
};

