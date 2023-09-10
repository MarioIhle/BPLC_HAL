#ifndef HAL_DIN11_h
#define HAL_DIN11_h

#include "Arduino.h"
#include "PCF8574.h"
#include "IOM_base.h"

//#define DEBUG_HAL_DIN11

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE PORTS
//-------------------------------------------------------------
typedef enum
{
    DIN11_CARD_1 = 0x20,
    DIN11_CARD_2 = 0x22,
    DIN11_CARD_3 = 0x21,
    DIN11_CARD_4 = 0x23,
    
    DIN11_CARD_COUNT = 4,

}e_DIN11_ADDRESS_t;

//DIGITAL INPUTS
typedef enum
{
    DI_PORT_1,
    DI_PORT_2,
    DI_PORT_3,
    DI_PORT_4,
    DI_PORT_5,
    DI_PORT_6,
    DI_PORT_7,
    DI_PORT_8,

    DI_PORT_COUNT,

}e_digitalInputPorts_t;

//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_DIN11 
{
    public:
    HAL_DIN11    ();
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7);
    HAL_DIN11    (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7, DigitalInput* P_PORT_8);

    void    begin();
    void    tick        ();    
    void    somePinOfsomePinCardChanged();

    private: 
    DigitalInput*   p_ports [DI_PORT_COUNT];    
    const uint8_t   PINS    [DI_PORT_COUNT] = {DI_PORT_4, DI_PORT_3, DI_PORT_2, DI_PORT_1, DI_PORT_5, DI_PORT_6, DI_PORT_7, DI_PORT_8};         

    int         f_somePinOfsomePinCardChanged;

    //Settings
    PCF8574     PCF;
    int         deviceAdress;
    int         usedPortCount;
 };
#endif