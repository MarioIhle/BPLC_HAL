#ifndef HAL_FUSE12_h
#define HAL_FUSE12_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"

//-------------------------------------------------------------
//I2C ADDRESSEN
typedef enum
{
    I2C_ADDRESS_FUSE12__ADDR_1 = 0xC0,
    I2C_ADDRESS_FUSE12__ADDR_2 = 0xC1,
    I2C_ADDRESS_FUSE12__ADDR_3 = 0xC2,
    I2C_ADDRESS_FUSE12__ADDR_4 = 0xC3,
    
    I2C_ADDRESS_FUSE12__COUNT = 4,
}e_I2C_ADDRSS_FUSE12_t;

//-------------------------------------------------------------
class HAL_FUSE12: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal constructor
                    HAL_FUSE12          ();
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
    e_I2C_ADDRSS_FUSE12_t   deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif