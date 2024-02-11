#ifndef HAL_DIN11_h
#define HAL_DIN11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"

#include "BPLC_ioBaseTypes.h"
#include "BPLC_LOG.h"
#include "I2C_check.h"
#include "HAL_interface.h"
#include "BPLC_ERRORS.h"
//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    DIN11_CARD_1_ADDRESS = 0x20,
    DIN11_CARD_2_ADDRESS = 0x22,
    DIN11_CARD_3_ADDRESS = 0x21,
    DIN11_CARD_4_ADDRESS = 0x23,
    
    DIN11_CARD_ADDRESS_COUNT = 4,

}e_DIN11_ADDRESS_t;

#define DIN11_CHANNEL_COUNT 8


//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
#define READ_TWO_TIMES 2

class HAL_DIN11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
    //Hal Interface
                    HAL_DIN11           (const e_DIN11_ADDRESS_t I2C_ADDRESS);
    void            init               ();
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode            (); 
        
    
    private:    
    //Settings
    PCF8574           PCF;
    e_DIN11_ADDRESS_t deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [DIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [DIN11_CHANNEL_COUNT] = {3, 2, 1, 0, 4, 5, 6, 7};      
    }channels;       
 };
#endif