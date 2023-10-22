#ifndef HAL_DIN11_h
#define HAL_DIN11_h

#include "Arduino.h"
#include "PCF8574.h"
#include "IOM_base.h"
#include "BPLC_TYPES.h"
#include "I2C_check.h"

//#define DEBUG_HAL_DIN11

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE PORTS
//-------------------------------------------------------------
typedef enum
{
    DIN11_CARD__1,
    DIN11_CARD__2,
    DIN11_CARD__3,
    DIN11_CARD__4,
    
    DIN11_CARD__COUNT = 4,

}e_DIN11_CARD_t;

typedef enum
{
    DIN11_CARD_1_ADDRESS = 0x20,
    DIN11_CARD_2_ADDRESS = 0x22,
    DIN11_CARD_3_ADDRESS = 0x21,
    DIN11_CARD_4_ADDRESS = 0x23,
    
    DIN11_CARD_ADDRESS_COUNT = 4,

}e_DIN11_ADDRESS_t;

//DIGITAL INPUTS
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

typedef enum
{
    DIN11_ERROR__NO_ERROR,
    DIN11_ERROR__I2C_CONNECTION_FAILED, //i2c verbindung fehlgeschlagen
    DIN11_ERROR__PORT_ALREADY_DEFINED,
    DIN11_ERROR__PORT_OVERFLOW,         //9/8 Ports definert

    DIN11_ERROR__COUNT,
}e_DIN11_ERROR_t;


//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_DIN11 
{
    public:
    HAL_DIN11 ();
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7);
    HAL_DIN11 (const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7, DigitalInput* P_PORT_8);

    void begin();
    void begin(const e_DIN11_ADDRESS_t I2C_ADDRESS);
    
    void mapObjectToPort(DigitalInput* P_PORT);

    void tick ();    
    void somePinOfsomeDinCardChanged();
    e_DIN11_ERROR_t getError();    

    private: 
    //Applikation  
    uint8_t f_somePinOfsomePinCardChanged;
    
    //Safety
    I2C_check       selfCheck;
    e_DIN11_ERROR_t errorCode;

    //Settings
    PCF8574     PCF;
    e_DIN11_ADDRESS_t deviceAddress;
    uint8_t           usedPorts;

    DigitalInput* p_ports   [DIN11_PORT__COUNT]; 
    const uint8_t PINS      [DIN11_PORT__COUNT] = {DIN11_PORT__4, DIN11_PORT__3, DIN11_PORT__2, DIN11_PORT__1, DIN11_PORT__5, DIN11_PORT__6, DIN11_PORT__7, DIN11_PORT__8};
 };
#endif