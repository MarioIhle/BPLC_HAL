#ifndef HAL_REL11_h
#define HAL_REL11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"
#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "BPLC_LOG.h"
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
    REL11_CARD_4_ADDRESS = 0x28,
    
    REL11_CARD_ADDRESS_COUNT = 4,
}e_REL11_ADDRESS_t;

typedef enum
{
    REL11_CHANNEL__1,
    REL11_CHANNEL__2,
    REL11_CHANNEL__3,   

    REL11_CHANNEL__COUNT,

}e_REL11_CHANNEL_t;

typedef enum
{
  REL_CHANNEL_STATE__NOT_USED,
  REL_CHANNEL_STATE__DIGITAL,

  REL_CHANNEL_STATE__COUNT,
}e_REL_CHANNEL_STATE_t;

//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_REL11 {

    public:
    HAL_REL11();

    void begin(const e_REL11_ADDRESS_t I2C_ADDRESS);
    e_BPLC_ERROR_t mapObjectToChannel(Output* P_OBJECT, const uint8_t CHANNEL);

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

    struct
    {
        e_REL_CHANNEL_STATE_t   state       [REL11_CHANNEL__COUNT];
        Output*                 p_object    [REL11_CHANNEL__COUNT];
        const uint8_t           PIN         [REL11_CHANNEL__COUNT] = {0, 1, 2};         
    }channels; 
    
};

#endif