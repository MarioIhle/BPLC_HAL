#ifndef HAL_NANO11_h
#define HAL_NANO11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"

//-------------------------------------------------------------
//I2C ADDRESSEN
typedef enum
{
    I2C_ADDRESS_NANO11__ADDR_1 = 0xB0,
    I2C_ADDRESS_NANO11__ADDR_2 = 0xB1,
    I2C_ADDRESS_NANO11__ADDR_3 = 0xB2,
    I2C_ADDRESS_NANO11__ADDR_4 = 0xB3,
    
    I2C_ADDRESS_NANO11__COUNT = 4,
}e_I2C_ADDRESS_NANO11_t;

//-------------------------------------------------------------
class HAL_NANO11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_NANO11          ();
    //Hal interface 
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    //Modul Error Interface
    e_BPLC_ERROR_t  getModulError       (){return this->getError();}
    void            resetModulError     (String FILE, const uint16_t LINE){this->resetError(FILE, LINE);}

  
    private:          
    //Settings  
    e_I2C_ADDRESS_NANO11_t   deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif