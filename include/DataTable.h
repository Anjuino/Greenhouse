#pragma once
#include "Arduino.h"

struct DataCrop{
    uint16_t GroundWet;
    uint16_t GroundDry;
    uint8_t AirWet;
    uint8_t AirDry;
    uint16_t Light;
};      


#define MINT
//#define FLOWER

#ifdef MINT
    extern DataCrop Mint;  // Объявление
#endif

