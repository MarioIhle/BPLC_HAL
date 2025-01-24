#ifndef HAL_AIN11_h
#define HAL_AIN11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include <Adafruit_ADS1X15.h>

//-------------------------------------------------------------
//Card definition
#define AIN11_ADDRESS_COUNT 4
#define AIN11_CHANNEL_COUNT 4
const uint8_t AIN11_I2C_ADDRESSES[AIN11_ADDRESS_COUNT] = {0x48, 0x49, 0x4A, 0x4B};

//-------------------------------------------------------------
class HAL_AIN11: public halInterface,  private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_AIN11               ();
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
    Adafruit_ADS1115    ADC;
    uint8_t             deviceAddress;
    adsGain_t           adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [AIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [AIN11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif