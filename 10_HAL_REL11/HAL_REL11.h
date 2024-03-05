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
    I2C_ADDRESS_REL11__ADDR_1 = 0x24,
    I2C_ADDRESS_REL11__ADDR_2 = 0x26,
    I2C_ADDRESS_REL11__ADDR_3 = 0x25,
    I2C_ADDRESS_REL11__ADDR_4 = 0x28,
    
    I2C_ADDRESS_REL11__COUNT = 4,
}e_I2C_ADDRESS_REL11_t;

#define REL11_CHANNEL_COUNT 3
//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_REL11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:    
    //Hal Interface
                    HAL_REL11           ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        (); 

    private:    
    //Settings
    PCF8574 PCF;
    e_I2C_ADDRESS_REL11_t deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [REL11_CHANNEL_COUNT];
        const uint8_t PIN         [REL11_CHANNEL_COUNT] = {0, 1, 2};         
    }channels; 
};

#endif