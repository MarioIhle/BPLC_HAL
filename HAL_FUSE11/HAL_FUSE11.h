#ifndef HAL_FUSE11_h
#define HAL_FUSE11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "IOM_Base.h"
#include "PCF8574.h"

//#define DEBUG_HAL_FUSE11

typedef enum
{
    FUSE11_CARD_1 = 0x20,
    FUSE11_CARD_2 = 0x21,
    FUSE11_CARD_3 = 0x22,
    FUSE11_CARD_4 = 0x23,
    FUSE11_CARD_5 = 0x24,
    FUSE11_CARD_6 = 0x25,
    FUSE11_CARD_7 = 0x26,
    FUSE11_CARD_8 = 0x27,
    
    FUSE11_CARD_COUNT = 8,

}e_FUSE11_ADDRESS_t;


typedef enum
{
    OUT_PORT_1,
    OUT_PORT_2,
    OUT_PORT_3,   
    OUT_PORT_4,

    OUT_PORT_COUNT,

}e_fuse11OutputPorts_t;

typedef enum
{
    IN_PORT_1,
    IN_PORT_2,
    IN_PORT_3,   
    IN_PORT_4,

    IN_PORT_COUNT,

}e_fuse11InputPorts_t;

class HAL_FUSE11 {

    public:
    HAL_FUSE11();
    HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, DigitalInput* P_T_F1);
    HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, DigitalInput* P_T_F1, DigitalInput* P_T_F2);
    HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, DigitalInput* P_T_F1, DigitalInput* P_T_F2, DigitalInput* P_T_F3);
    HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, DigitalInput* P_T_F1, DigitalInput* P_T_F2, DigitalInput* P_T_F3, DigitalInput* P_T_F4);
   
    void        tick();
    void        begin();
    
    void        somePinOfsomeDinCardChanged();
    uint8_t     isAFuseBroken();

    
    private:
    DigitalInput*   p_DI    [IN_PORT_COUNT];
    Output*         p_DO    [OUT_PORT_COUNT];    
    
    struct 
    {
        const uint8_t   input    [IN_PORT_COUNT]    = {4, 5, 6, 7};    
        const uint8_t   output   [OUT_PORT_COUNT]   = {0, 1, 2, 4};    
    }pins;
    
    PCF8574 PCF;
    int     usedPortCount;
    uint8_t f_somePinOfsomePinCardChanged;
};

#endif