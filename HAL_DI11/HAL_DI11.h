#ifndef HAL_DI11_h
#define HAL_DI11_h

#include "Arduino.h"
#include "PCF8574.h"
#include "IOM_base.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE PORTS
//-------------------------------------------------------------
typedef enum
{
    DI11_CARD_1 = 0x20,
    DI11_CARD_2 = 0x22,
    DI11_CARD_3 = 0x21,
    DI11_CARD_4 = 0x23,
    
    DI11_CARD_COUNT = 4,

}e_DI11_ADDRESS_t;

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
//HAL_DI11 KLASSE
//-------------------------------------------------------------
class HAL_DI11 
{
    public:
    HAL_DI11    ();
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7);
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7, DigitalInput* P_PORT_8);

    void    begin(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1);
    void    tick        ();    
    void    somePinOfsomeDI11CardChanged();

    private: 
    DigitalInput*   p_ports [DI_PORT_COUNT];    
    const uint8_t   PINS    [DI_PORT_COUNT] = {DI_PORT_4, DI_PORT_3, DI_PORT_2, DI_PORT_1, DI_PORT_5, DI_PORT_6, DI_PORT_7, DI_PORT_8};         

    int             usedPortCount;
    bool            f_somePinOfsomeDI11CardChanged;
 };
#endif