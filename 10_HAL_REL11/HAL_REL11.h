#ifndef HAL_REL11_h
#define HAL_REL11_h

//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCF8574.h"

//-------------------------------------------------------------
//I2C ADDRESSEN
typedef enum
{
    I2C_ADDRESS_REL11__ADDR_1 = 0x24,
    I2C_ADDRESS_REL11__ADDR_2 = 0x26,
    I2C_ADDRESS_REL11__ADDR_3 = 0x25,
    I2C_ADDRESS_REL11__ADDR_4 = 0x28,
    
    I2C_ADDRESS_REL11__COUNT = 4,
}e_I2C_ADDRESS_REL11_t;

#define REL11_CHANNEL_COUNT 3

//-------------------------------------------------------------
class HAL_REL11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:    
    //Hal Interface
                    HAL_REL11           ();
    //Hal interface 
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    //Modul Error Interface
    e_BPLC_ERROR_t  getModulError       ();
    void            resetModulError     (String FILE, const uint16_t LINE){this->resetError(FILE, LINE);}


    private:    
    //Settings
    PCF8574 PCF;
    e_I2C_ADDRESS_REL11_t deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [REL11_CHANNEL_COUNT];
        const uint8_t PIN         [REL11_CHANNEL_COUNT] = {0, 1, 2};         
    }channels; 
};
#endif