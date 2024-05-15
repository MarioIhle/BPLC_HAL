#ifndef HAL_DIN11_h
#define HAL_DIN11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCA9685.h"
#include "PCF8574.h"

//-------------------------------------------------------------
//Card definition
#define DIN11_ADDRESS_COUNT 4
#define DIN11_CHANNEL_COUNT 8
const uint8_t DIN11_I2C_ADDRESSES[DIN11_ADDRESS_COUNT] = {0x20, 0x22, 0x21, 0x23};

//-------------------------------------------------------------
class HAL_DIN11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal Interface
                    HAL_DIN11               ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    ();        
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}
   
    
    private:    
    //Settings
    PCF8574 PCF;
    uint8_t deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [DIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [DIN11_CHANNEL_COUNT] = {3, 2, 1, 0, 4, 5, 6, 7};      
    }channels;       
 };
#endif