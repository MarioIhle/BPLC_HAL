#ifndef HAL_TMP11_h
#define HAL_TMP11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "MCP342x.h"

//-------------------------------------------------------------
//Card definition
#define TMP11_ADDRESS_COUNT 4
#define TMP11_CHANNEL_COUNT 4
const uint8_t TMP11_I2C_ADDRESSES[TMP11_ADDRESS_COUNT] = {0x68, 0x6A, 0x6C, 0x6E};

//-------------------------------------------------------------
class HAL_TMP11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_TMP11               ();
    ///Hal interface 
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
    uint8_t     deviceAddress;
    MCP342x     adc;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif