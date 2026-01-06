#ifndef HAL_TMP11_h
#define HAL_TMP11_h
/*

ACHTUNG INFO: BEI NUTZUNG ÜBER USB KANN ES ZU TEMPERATUR ABWEICHUNGEN KOMMEN
-> WERTE ABGESTIMMT AUF 5V VERSORUNG, bei USB 4,7V

-> Bei DIPschalter änderung immer die Versorungspannung neustarten, MCP liest nur 1x bei Systemstart die Addresspins

*/
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "MCP342x.h"

//-------------------------------------------------------------
//NANOVOLT PRO GRAD 
#define nVperDegreePT100              18760  //nV
#define UabAtZeroDegrePT100_nV    -40504050  //nV
#define nVperDegreePT1000            177187  //nV

//-------------------------------------------------------------
//Card definition
#define TMP11_ADDRESS_COUNT 4
#define TMP11_CHANNEL_COUNT 4
const uint8_t TMP11_I2C_ADDRESSES[TMP11_ADDRESS_COUNT] = {0x6E, 0x6C, 0x6A, 0x68};

//-------------------------------------------------------------
class HAL_TMP11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_TMP11               ();
    ///Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    (const bool READ_INPUTS);        
    void            controlCommand          (const e_EC_COMMAND_t COMMAND);   
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}


  
    private:          
    //Settings
    MCP342x     adc;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {0, 1, 3, 2};   
    }channels;         
 };
#endif