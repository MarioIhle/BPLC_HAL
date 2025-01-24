#ifndef HAL_REL11_h
#define HAL_REL11_h

//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCF8574.h"

//-------------------------------------------------------------
//Card definition
#define REL11_ADDRESS_COUNT 4
#define REL11_CHANNEL_COUNT 3
const uint8_t REL11_I2C_ADDRESSES[REL11_ADDRESS_COUNT] = {0x24, 0x26, 0x25, 0x28};

//-------------------------------------------------------------
class HAL_REL11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:    
    //Hal Contructor
                    HAL_REL11               ();
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
    PCF8574 PCF;
    uint8_t deviceAddress;

    struct
    {
        IO_Interface* p_ioObject  [REL11_CHANNEL_COUNT];
        const uint8_t PIN         [REL11_CHANNEL_COUNT] = {0, 1, 2};         
    }channels; 
};
#endif