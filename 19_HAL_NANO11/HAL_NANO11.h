#ifndef HAL_NANO11_h
#define HAL_NANO11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"

//-------------------------------------------------------------
//Card definition
#define NANO11_ADDRESS_COUNT 10
#define NANO11_CHANNEL_COUNT 1
const uint8_t NANO11_I2C_ADDRESSES[NANO11_ADDRESS_COUNT] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9};

//-------------------------------------------------------------
class HAL_NANO11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal constructor
                    HAL_NANO11              ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    ();        
    void            controlCommand          (const e_EC_COMMAND_t COMMAND);   
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

  
    private:          
    //Settings  
    uint8_t   deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif