#ifndef HAL_REL11_h
#define HAL_REL11_h
#include "Arduino.h"
#include "IOM_Base.h"
#include "PCF8574.h"

typedef enum
{
    REL11_CARD_1 = 0x3C,
    REL11_CARD_2 = 0x3D,
    REL11_CARD_3 = 0x3E,
    REL11_CARD_4 = 0x3F,
    
    REL11_CARD_COUNT = 4,

}e_REL11_ADDRESS_t;

typedef enum
{
    REL_PORT_1,
    REL_PORT_2,
    REL_PORT_3,   

    REL_PORT_COUNT,

}e_relaisPorts_t;


class HAL_REL11 {

    public:
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS);
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1);
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2);
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3);

    void        tick();
    
    private:    
    Output*         p_REL   [REL_PORT_COUNT];
    const uint8_t   pins    [REL_PORT_COUNT] = {REL_PORT_1, REL_PORT_2, REL_PORT_3};  

    uint8_t usedPortCount;     
};

#endif