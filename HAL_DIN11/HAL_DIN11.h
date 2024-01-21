#ifndef HAL_DIN11_h
#define HAL_DIN11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"
#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE DEBUGGING
//-------------------------------------------------------------
//#define DEBUG_HAL_DIN11

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    DIN11_CARD__1,
    DIN11_CARD__2,
    DIN11_CARD__3,
    DIN11_CARD__4,
    
    DIN11_CARD__MAX = 4,

}e_DIN11_CARD_t;

typedef enum
{
    DIN11_CARD_1_ADDRESS = 0x20,
    DIN11_CARD_2_ADDRESS = 0x22,
    DIN11_CARD_3_ADDRESS = 0x21,
    DIN11_CARD_4_ADDRESS = 0x23,
    
    DIN11_CARD_ADDRESS_COUNT = 4,

}e_DIN11_ADDRESS_t;

typedef enum
{
    DIN11_PORT__1,
    DIN11_PORT__2,
    DIN11_PORT__3,
    DIN11_PORT__4,
    DIN11_PORT__5,
    DIN11_PORT__6,
    DIN11_PORT__7,
    DIN11_PORT__8,

    DIN11_PORT__COUNT,

}e_DIN11_PORTS_t;

//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_DIN11 
{
    public:
    HAL_DIN11 ();
    void begin(const e_DIN11_ADDRESS_t I2C_ADDRESS);
    e_BPLC_ERROR_t mapObjectToNextFreePort(DigitalInput* P_OBJECT);
    e_BPLC_ERROR_t mapObjectToSpecificPort(DigitalInput* P_OBJECT, const e_DIN11_PORTS_t PORT);
    
    void            tick ();    
    void            somePinOfsomeDinCardChanged();
    e_BPLC_ERROR_t  getError();    
    
    void isrFastRead();
    private: 
    //Applikation  
    uint8_t f_somePinOfsomeDinCardChanged;
    
    //Safety
    I2C_check       selfCheck;
    e_BPLC_ERROR_t  errorCode;

    //Settings
    PCF8574           PCF;
    e_DIN11_ADDRESS_t deviceAddress;

    //Object handling
    struct
    {
        e_PORT_USEAGE_t used    [DIN11_PORT__COUNT];
        DigitalInput*   p_object[DIN11_PORT__COUNT];         
    }ports;    
    
    const uint8_t   PIN     [DIN11_PORT__COUNT] = {3, 2, 1, 0, 4, 5, 6, 7};
 };
#endif