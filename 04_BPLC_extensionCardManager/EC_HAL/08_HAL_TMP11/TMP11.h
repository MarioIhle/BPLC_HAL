#ifndef HAL_TMP11_h
#define HAL_TMP11_h
//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"

#include "BPLC_ioBaseTypes.h"
#include "BPLC_LOG.h"
#include "BPLC_ERRORS.h"

#include "EC_HAL/00_API/I2C_check/I2C_check.h"
#include "EC_HAL/00_API/HAL_interface.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    TMP11_CARD_1_ADDRESS = 0x48,
    TMP11_CARD_2_ADDRESS = 0x49,
    TMP11_CARD_3_ADDRESS = 0x4A,
    TMP11_CARD_4_ADDRESS = 0x4B,
    
    TMP11_CARD_ADDRESS__COUNT = 4,
}e_TMP11_ADDRESS_t;

#define TMP11_CHANNEL_COUNT 4
//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_TMP11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_TMP11           (const e_TMP11_ADDRESS_t I2C_ADDRESS);
    void            init                ();
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();

  
    private:          
    //Settings
    Adafruit_ADS1115    ADC;
    e_TMP11_ADDRESS_t   deviceAddress;
    adsGain_t           adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif