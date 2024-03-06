#ifndef HAL_TMP11_h
#define HAL_TMP11_h
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
    I2C_ADDRESS_TMP11__ADDR_1 = 0x68,
    I2C_ADDRESS_TMP11__ADDR_2 = 0x6A,
    I2C_ADDRESS_TMP11__ADDR_3 = 0x6C,
    I2C_ADDRESS_TMP11__ADDR_4 = 0x6E,
    
    I2C_ADDRESS_TMP11__COUNT = 4,
}e_I2C_ADDRESS_TMP11_t;

#define TMP11_CHANNEL_COUNT 4
//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_TMP11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_TMP11           ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();

  
    private:          
    //Settings
    Adafruit_ADS1115        ADC;
    e_I2C_ADDRESS_TMP11_t   deviceAddress;
    adsGain_t               adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif