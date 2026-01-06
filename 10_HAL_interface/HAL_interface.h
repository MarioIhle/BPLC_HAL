#ifndef HAL_interface_h
#define HAL_interface_h

#include "Arduino.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_errorHandler.h"
#include "I2C_check.h"
#include "BPLC_logPrint.h"


typedef enum 
{    
    EC_ADDR_1,
    EC_ADDR_2,
    EC_ADDR_3,
    EC_ADDR_4,
    EC_ADDR_5,
    EC_ADDR_6,
    EC_ADDR_7,
    EC_ADDR_8,

    EC_ADDR_NOT_DEFINED
}e_EC_ADDR_t;

typedef enum
{
    EC_CHANNEL_1 = 1, 
    EC_CHANNEL_2, 
    EC_CHANNEL_3, 
    EC_CHANNEL_4, 
    EC_CHANNEL_5, 
    EC_CHANNEL_6, 
    EC_CHANNEL_7, 
    EC_CHANNEL_8,

    EC_CHANNEL_NOT_DEFINED
}e_EC_CHANNEL_t;

typedef enum
{
    EC_COMMAND__SELFTEST,   
    EC_COMMAND__MOT11_START_CURRENT_TUNING,
    EC_COMMAND__ENABLE_DEBUG_OUTPUT,
    EC_COMMAND__DISABLE_ERROR_DETECTION,

    EC_COMMAND__COUNT

}e_EC_COMMAND_t;

class halInterface: public BPLC_moduleErrorInterface
{
    public:
    virtual void init                   (const e_EC_ADDR_t ADDR) = 0;
    virtual void tick                   (const bool READ_INPUTS) = 0;
    virtual void mapObjectToChannel     (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL) = 0;
    virtual void controlCommand         (const e_EC_COMMAND_t COMMAND) = 0;

    //public
    uint8_t     i2cAddress;
    e_EC_ADDR_t bplcAddress;
    bool        debugOutputEnabled;
};
#endif