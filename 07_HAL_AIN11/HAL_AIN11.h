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
#include "BPLC_LOG.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    AIN11_CARD_1_ADDRESS = 0x48,
    AIN11_CARD_2_ADDRESS = 0x49,
    AIN11_CARD_3_ADDRESS = 0x4A,
    AIN11_CARD_4_ADDRESS = 0x4B,
    
    AIN11_CARD_ADDRESS__COUNT = 4,
}e_AIN11_ADDRESS_t;

#define AIN11_CHANNEL_COUNT 4
//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_AIN11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_AIN11           (const e_AIN11_ADDRESS_t I2C_ADDRESS);
    void            init                ();
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();

    void            setSampleTime       (const uint64_t SAMPLE_TIME);

    private:          
    //Settings
    Adafruit_ADS1115    ADC;
    e_AIN11_ADDRESS_t   deviceAddress;
    adsGain_t           adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [AIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [AIN11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif