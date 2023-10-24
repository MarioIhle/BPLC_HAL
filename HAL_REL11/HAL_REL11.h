#ifndef HAL_REL11_h
#define HAL_REL11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "IOM_Base.h"
#include "PCF8574.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE DEBUGGING
//-------------------------------------------------------------
//#define DEBUG_HAL_REL11 

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    REL11_CARD__1,
    REL11_CARD__2,
    REL11_CARD__3,
    REL11_CARD__4,
    
    REL11_CARD__MAX = 4,

}e_REL11_CARD_t;

typedef enum
{
    REL11_CARD_1_ADDRESS = 0x24,
    REL11_CARD_2_ADDRESS = 0x26,
    REL11_CARD_3_ADDRESS = 0x25,
    REL11_CARD_4_ADDRESS = 0x27,
    
    REL11_CARD_ADDRESS_COUNT = 4,
}e_REL11_ADDRESS_t;

typedef enum
{
    REL11_PORT__1,
    REL11_PORT__2,
    REL11_PORT__3,   

    REL11_PORT__COUNT,

}e_REL11_PORTS_t;

//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_REL11 {

    public:
    HAL_REL11();
    HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS, Output* P_REL1);
    HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS, Output* P_REL1, Output* P_REL2);
    HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3);

    void begin();
    void begin(const e_REL11_ADDRESS_t I2C_ADDRESS);
    void mapObjectToPort(Output* P_OBJECT);

    void tick();
    
    e_BPLC_ERROR_t getError();

    private:    
    //Applikation

    //Safety
    I2C_check       selfCheck;
    e_BPLC_ERROR_t  errorCode; 

    //Settings
    PCF8574 PCF;
    e_REL11_ADDRESS_t   deviceAddress;
    uint8_t             usedPorts; 

    Output*         p_REL   [REL11_PORT__COUNT];
    const uint8_t   PINS    [REL11_PORT__COUNT] = {0, 1, 2};  
};

#endif