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
    HAL_DI11    (const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1 = nullptr, DigitalInput* P_PORT_2 = nullptr, DigitalInput* P_PORT_3 = nullptr, DigitalInput* P_PORT_4 = nullptr, DigitalInput* P_PORT_5 = nullptr, DigitalInput* P_PORT_6 = nullptr, DigitalInput* P_PORT_7 = nullptr, DigitalInput* P_PORT_8 = nullptr);

    void    begin(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1);
    void    tick        ();    
    void    somePinOfsomeDI11CardChanged();

    private: 
    int         usedPortCount;

    DigitalInput*   p_ports [DI_PORT_COUNT];    
    const uint8_t   PINS    [DI_PORT_COUNT] = {DI_PORT_4, DI_PORT_3, DI_PORT_2, DI_PORT_1, DI_PORT_5, DI_PORT_6, DI_PORT_7, DI_PORT_8};         

    int             usedPortCount;
    bool            f_somePinOfsomeDI11CardChanged;
 };
#endif