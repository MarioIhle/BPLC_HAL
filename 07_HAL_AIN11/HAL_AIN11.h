#ifndef HAL_AIN11_h
#define HAL_AIN11_h
//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include <Adafruit_ADS1X15.h>
#include "SpecialFunctions.h"

#include "BPLC_ioBaseTypes.h"
#include "HAL_interface.h"
#include "BPLC_ERRORS.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    I2C_ADDRESS_AIN11__ADDR_1 = 0x48,
    I2C_ADDRESS_AIN11__ADDR_2 = 0x49,
    I2C_ADDRESS_AIN11__ADDR_3 = 0x4A,
    I2C_ADDRESS_AIN11__ADDR_4 = 0x4B,
    
    I2C_ADDRESS_AIN11__COUNT = 4,
}e_I2C_ADDRESS_AIN11_t;

#define AIN11_CHANNEL_COUNT 4
//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_AIN11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_AIN11           ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();


    private:          
    //Settings
    Adafruit_ADS1115        ADC;
    e_I2C_ADDRESS_AIN11_t   deviceAddress;
    adsGain_t               adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [AIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [AIN11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif