#ifndef HAL_REL11_h
#define HAL_REL11_h
#include "Arduino.h"
#include "IOM_Base.h"
#include "PCF8574.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "I2C_check.h"

//#define DEBUG_HAL_REL11 

typedef enum
{
    REL11_CARD_1 = 0x24,
    REL11_CARD_2 = 0x26,
    REL11_CARD_3 = 0x25,
    REL11_CARD_4 = 0x27,
    
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
    HAL_REL11();
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1);
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2);
    HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3);

    e_BPLC_ERROR_t begin();
    void          tick();
    
    e_BPLC_ERROR_t getError();

    private:    
    //Applikation

    //Safety
    I2C_check   selfCheck;
    bool        f_error; 

    //Settings
    Output*         p_REL   [REL_PORT_COUNT];
    const uint8_t   pins    [REL_PORT_COUNT] = {REL_PORT_1, REL_PORT_2, REL_PORT_3};  
    
    PCF8574 PCF;
    int deviceAddress;
    int usedPorts; 
};

#endif