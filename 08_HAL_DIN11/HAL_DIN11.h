#ifndef HAL_DIN11_h
#define HAL_DIN11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"

#include "BPLC_ioBaseTypes.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"
#include "HAL_interface.h"
#include "BPLC_ERRORS.h"
//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    I2C_ADDRESS_DIN11__ADDR_1 = 0x20,
    I2C_ADDRESS_DIN11__ADDR_2 = 0x22,
    I2C_ADDRESS_DIN11__ADDR_3 = 0x21,
    I2C_ADDRESS_DIN11__ADDR_4 = 0x23,
    
    I2C_ADDRESS_DIN11__COUNT = 4,

}e_I2C_ADDRESS_DIN11_t;

#define DIN11_CHANNEL_COUNT 8


//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
#define READ_TWO_TIMES 2

class HAL_DIN11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
    //Hal Interface
                    HAL_DIN11           ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        (); 
        
    
    private:    
    //Settings
    PCF8574           PCF;
    e_I2C_ADDRESS_DIN11_t deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [DIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [DIN11_CHANNEL_COUNT] = {3, 2, 1, 0, 4, 5, 6, 7};      
    }channels;       
 };
#endif