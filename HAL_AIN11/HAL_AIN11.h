#ifndef HAL_AIN11_h
#define HAL_AIN11_h
//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include <Adafruit_ADS1X15.h>
#include "SpecialFunctions.h"
#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "BPLC_LOG.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    AIN11_CARD__1,
    AIN11_CARD__2,
    AIN11_CARD__3,
    AIN11_CARD__4,
    
    AIN11_CARD__MAX,
}e_AIN11_CARD_t;

typedef enum
{
    AIN11_CARD_1_ADDRESS = 0x48,
    AIN11_CARD_2_ADDRESS = 0x49,
    AIN11_CARD_3_ADDRESS = 0x4A,
    AIN11_CARD_4_ADDRESS = 0x4B,
    
    AIN11_CARD_ADDRESS__COUNT = 4,
}e_AIN11_ADDRESS_t;

typedef enum
{   
    AIN11_CHANNEL__1,
    AIN11_CHANNEL__2,
    AIN11_CHANNEL__3,
    AIN11_CHANNEL__4,

    AIN11_CHANNEL__COUNT,
}e_AIN11_CHANNEL_t;

//IO Channelstate
typedef enum
{
    AIN_CHANNEL__NOT_USED,
    AIN_CHANNEL__ANALOG,   

    AIN_CHANNEL__COUNT,
}e_AIN_CHANNEL_STATE_t;


//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_AIN11 
{
    public:
    HAL_AIN11();
    void            begin             (const e_AIN11_ADDRESS_t I2C_ADDRESS, const uint16_t READ_INTERVAL = 1000);
    e_BPLC_ERROR_t  mapObjectToChannel(AnalogInput* P_OBJECT, const e_AIN11_CHANNEL_t CHANNEL);

    void           tick    ();  
    e_BPLC_ERROR_t getError();  

    private: 
    //Applikation

    //Safety
    I2C_check       selfCheck;
    e_BPLC_ERROR_t  errorCode;          
 
    //Settings
    Adafruit_ADS1115    ADC;
    e_AIN11_ADDRESS_t   deviceAddress;
    adsGain_t           adcGain;
    Timeout             to_read;

    //Object handling
    struct
    {
        e_AIN_CHANNEL_STATE_t   state                   [AIN11_CHANNEL__COUNT];
        AnalogInput*            p_analogInputInstance   [AIN11_CHANNEL__COUNT];   
        const uint8_t           PIN                     [AIN11_CHANNEL__COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif