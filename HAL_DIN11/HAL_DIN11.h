#ifndef HAL_DIN11_h
#define HAL_DIN11_h

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
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    DIN11_CARD__1,
    DIN11_CARD__2,
    DIN11_CARD__3,
    DIN11_CARD__4,
    
    DIN11_CARD__MAX = 4,

}e_DIN11_CARD_t;

typedef enum
{
    DIN11_CARD_1_ADDRESS = 0x20,
    DIN11_CARD_2_ADDRESS = 0x22,
    DIN11_CARD_3_ADDRESS = 0x21,
    DIN11_CARD_4_ADDRESS = 0x23,
    
    DIN11_CARD_ADDRESS_COUNT = 4,

}e_DIN11_ADDRESS_t;

typedef enum
{
    DIN11_CHANNEL__1,
    DIN11_CHANNEL__2,
    DIN11_CHANNEL__3,
    DIN11_CHANNEL__4,
    DIN11_CHANNEL__5,
    DIN11_CHANNEL__6,
    DIN11_CHANNEL__7,
    DIN11_CHANNEL__8,

    DIN11_CHANNEL__COUNT,

}e_DIN11_CHANNEL_t;

typedef enum
{
    DIN_CHANNEL_STATE__NOT_USED,
    DIN_CHANNEL_STATE__DIGITAL,
    DIN_CHANNEL_STATE__COUNTER,

    DIN_CHANNEL_STATE__COUNT,
}e_DIN_CHANNEL_STATE_t;


//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
#define READ_TWO_TIMES 2

class HAL_DIN11 
{
    public:
    HAL_DIN11 ();
    void            begin               (const e_DIN11_ADDRESS_t I2C_ADDRESS);
    e_BPLC_ERROR_t  mapObjectToChannel  (DigitalInput*  P_OBJECT, const e_DIN11_CHANNEL_t CHANNEL);
    e_BPLC_ERROR_t  mapObjectToChannel  (rpmSensor*     P_OBJECT, const e_DIN11_CHANNEL_t CHANNEL);
    
    void            tick ();        
    e_BPLC_ERROR_t  getError();    
    
    
    private:    
    //Safety
    I2C_check       selfCheck;
    e_BPLC_ERROR_t  errorCode;

    //Settings
    PCF8574           PCF;
    e_DIN11_ADDRESS_t deviceAddress;

    //Object handling
    struct
    {
        e_DIN_CHANNEL_STATE_t    channelState[DIN11_CHANNEL__COUNT];
        DigitalInput*               p_digital   [DIN11_CHANNEL__COUNT];   
        rpmSensor*                  p_counter   [DIN11_CHANNEL__COUNT];   
        const uint8_t               PIN         [DIN11_CHANNEL__COUNT] = {3, 2, 1, 0, 4, 5, 6, 7};      
    }channels;       
 };
#endif