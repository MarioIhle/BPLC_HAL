#ifndef HAL_NANO11_h
#define HAL_NANO11_h
//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "BPLC_ioBaseTypes.h"
#include "HAL_interface.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    I2C_ADDRESS_NANO11__ADDR_1 = 0xB0,
    I2C_ADDRESS_NANO11__ADDR_2 = 0xB1,
    I2C_ADDRESS_NANO11__ADDR_3 = 0xB2,
    I2C_ADDRESS_NANO11__ADDR_4 = 0xB3,
    
    I2C_ADDRESS_NANO11__COUNT = 4,
}e_I2C_ADDRESS_NANO11_t;

//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_NANO11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_NANO11          ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();

  
    private:          
    //Settings  
    e_I2C_ADDRESS_NANO11_t   deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif