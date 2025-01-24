#ifndef HAL_FUSE12_h
#define HAL_FUSE12_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "BPLC_I2C_Nodes.h"
//-------------------------------------------------------------
//Card definition
#define FUSE12_ADDRESS_COUNT 16
#define FUSE12_CHANNEL_COUNT 4
const uint8_t FUSE12_I2C_ADDRESSES[FUSE12_ADDRESS_COUNT] = {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};

//-------------------------------------------------------------
class HAL_FUSE12: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal constructor
                    HAL_FUSE12              ();
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
    uint8_t deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_inObject  [FUSE12_CHANNEL_COUNT];   
        IO_Interface* p_outObject [FUSE12_CHANNEL_COUNT];  
    }channels;         
 };
#endif