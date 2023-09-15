#ifndef HAL_AIN11_h
#define HAL_AIN11_h

#include "Arduino.h"
#include "IOM_base.h"
#include <Adafruit_ADS1X15.h>
#include "SpecialFunctions.h"


//#define DEBUG_HAL_AIN11

typedef enum
{
    AIN11_CARD_1 = 0x48,
    AIN11_CARD_2 = 0x49,
    AIN11_CARD_3 = 0x4A,
    AIN11_CARD_4 = 0x4B,
    
    AIN11_CARD_COUNT = 4,
}e_AIN11_ADDRESS_t;


//__________________________________
//ANALOG INPUTS
typedef enum
{   
    AI_PORT_1,
    AI_PORT_2,
    AI_PORT_3,
    AI_PORT_4,

    AI_PORT_COUNT,
}e_analogInputPorts_t;


class HAL_AIN11 
{
    public:
    HAL_AIN11    ();
    HAL_AIN11    (const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1);
    HAL_AIN11    (const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2);
    HAL_AIN11    (const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_3);
    HAL_AIN11    (const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_3, AnalogInput* P_PORT_4);

    void    begin   (const uint16_t READ_INTERVAL = 1000);
    void    tick    ();  

    private: 
    AnalogInput*   p_ports [AI_PORT_COUNT];    
    const uint8_t  PINS    [AI_PORT_COUNT] = {AI_PORT_3, AI_PORT_4, AI_PORT_2, AI_PORT_1};         
 
    //Settings
    Adafruit_ADS1115    ADC;
    int                 deviceAdress;
    int                 usedPortCount;
    Timeout             to_read;
 };

#endif