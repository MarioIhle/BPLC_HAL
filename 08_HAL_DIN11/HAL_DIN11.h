#ifndef HAL_DIN11_h
#define HAL_DIN11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCA9685.h"
#include "PCF8574.h"

//-------------------------------------------------------------
//I2C ADDRESSEN
typedef enum
{
    I2C_ADDRESS_DIN11__ADDR_1 = 0x20,
    I2C_ADDRESS_DIN11__ADDR_2 = 0x22,
    I2C_ADDRESS_DIN11__ADDR_3 = 0x21,
    I2C_ADDRESS_DIN11__ADDR_4 = 0x23,
    
    I2C_ADDRESS_DIN11__COUNT = 4,

}e_I2C_ADDRESS_DIN11_t;

//-------------------------------------------------------------
#define DIN11_CHANNEL_COUNT 8

//-------------------------------------------------------------
class HAL_DIN11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal Interface
                    HAL_DIN11               ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                    ();        
    //Modul Error Interface   
    uint8_t         getModuleErrorCount           ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}
   
    
    private:    
    //Settings
    PCF8574                 PCF;
    e_I2C_ADDRESS_DIN11_t   deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [DIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [DIN11_CHANNEL_COUNT] = {3, 2, 1, 0, 4, 5, 6, 7};      
    }channels;       
 };
#endif