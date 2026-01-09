#ifndef HAL_MOT11_h
#define HAL_MOT11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "BPLC_I2C_Nodes.h"
//-------------------------------------------------------------
//Card definition
#define MOT11_ADDRESS_COUNT 4
#define MOT11_CHANNEL_COUNT 1
const uint8_t MOT11_I2C_ADDRESSES[MOT11_ADDRESS_COUNT] = {0xA1, 0xA2, 0xA3, 0xA4};

//DEBUGGING
//#define DEBUG_HAL_MOT11

//State auf MOT11
typedef enum
{
  MOT11_DEVICE_STATE__INIT,
  MOT11_DEVICE_STATE__RUNNING,
  MOT11_DEVICE_STATE__SAFE_STATE,
  MOT11_DEVICE_STATE__AUTOTUNING,

  MOT11_DEVICE_STATE__COUNT,
}e_MOT11_DEVICE_STATE_t;

//Commandos
typedef enum
{
  MOT11_I2C_KEY__DRIVE_COMMAND,
  MOT11_I2C_KEY__GET_DRIVE_STATE,
  MOT11_I2C_KEY__START_CURRENT_AUTOTUNING,

  MOT11_I2C_KEY__COUNT,
}e_MOT11_I2C_KEY_t;

//Datenpaket
#pragma pack (push, 1)
typedef union 
{   
  struct 
  {
    uint8_t key;          //1
    uint8_t deviceState;  //1+1 = 2
    uint8_t direction;    //2+1 = 3
    uint8_t speed;        //3+1 = 4
    uint8_t error;        //4+1 = 5
    float   current;      //5+4 = 9
  }extract; 

  uint8_t data[sizeof(extract)];

}u_MOT11_DATA_FRAME_t;
#pragma pack (pop)

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
    bool            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    (const bool READ_INPUTS);   
    void            controlCommand          (const e_EC_COMMAND_t COMMAND);        
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

  
    private:
    //Settings    
    uint8_t i2cAddress;   

    e_MOT11_DEVICE_STATE_t  state;
    e_BPLC_ERROR_t          error;   

    //I2C Kommunikation
    void startCurrentAutotuning ();
    void sendDriveCommand       (const u_HAL_DATA_t DRIVE_PARAMETER);
    void requestDriveParameter  ();
  
    Timeout to_parameterPoll;     

    //Object handling
    struct 
    {
      IO_Interface*           p_ioObject;   
    }channels;

    I2C_BPLC_Master i2c;
};
#endif