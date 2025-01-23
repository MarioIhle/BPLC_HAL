#ifndef HAL_MOT11_h
#define HAL_MOT11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "MOT11_I2C_COMMON_TYPES.h"
#include "BPLC_I2C_Nodes.h"
//-------------------------------------------------------------
//Card definition
#define MOT11_ADDRESS_COUNT 4
#define MOT11_CHANNEL_COUNT 1

//DEBUGGING
//#define DEBUG_HAL_MOT11
//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_MOT11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal Constructor
                    HAL_MOT11               ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    ();        
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

    //Spezifisch
    void            startCurrentAutotuning  ();  
    
  
    private:
    //Settings    
    uint8_t deviceAddress;   

    e_MOT11_DEVICE_STATE_t  state;
    e_BPLC_ERROR_t          error;   

    //I2C Kommunikation
    void sendDriveCommand     (const u_HAL_DATA_t DRIVE_PARAMETER);
    void requestDriveParameter();
  
    Timeout to_parameterPoll;     

    //Object handling
    struct 
    {
      IO_Interface*           p_ioObject;   
    }channels;

    I2C_BPLC_Master i2c;
};
#endif