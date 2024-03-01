#ifndef HAL_REL11_h
#define HAL_REL11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"

#include "HAL_interface.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_ERRORS.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE DEBUGGING
//-------------------------------------------------------------
typedef enum
{
    REL11_CARD_1_ADDRESS = 0x24,
    REL11_CARD_2_ADDRESS = 0x26,
    REL11_CARD_3_ADDRESS = 0x25,
    REL11_CARD_4_ADDRESS = 0x28,
    
    REL11_CARD_ADDRESS_COUNT = 4,
}e_REL11_ADDRESS_t;

#define REL11_CHANNEL_COUNT 3
//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_REL11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:    
    //Hal Interface
                    HAL_REL11           (const e_REL11_ADDRESS_t I2C_ADDRESS);
    void            init                ();
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        (); 

    private:    
    //Settings
    PCF8574 PCF;
    e_REL11_ADDRESS_t   deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [REL11_CHANNEL_COUNT];
        const uint8_t PIN         [REL11_CHANNEL_COUNT] = {0, 1, 2};         
    }channels; 
};

#endif