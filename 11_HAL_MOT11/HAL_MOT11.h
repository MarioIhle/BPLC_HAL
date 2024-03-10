#ifndef HAL_MOT11_h
#define HAL_MOT11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCA9685.h"

//-------------------------------------------------------------
//Card definition
#define MOT11_ADDRESS_COUNT 10
#define MOT11_CHANNEL_COUNT 1
const uint8_t MOT11_I2C_ADDRESSES[MOT11_ADDRESS_COUNT] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9};

//-------------------------------------------------------------
//APPLIKATION
//-------------------------------------------------------------
#define ACK 0x06
#define NAK 0x15

//State auf MOT11
typedef enum
{
  deviceState_init,
  deviceState_running,
  deviceState_safeState,
  deviceState_autotuning,

  deviceState_count,
}e_deviceState_t;

typedef enum
{
  //setter 
  mot11_i2c_key__driveCommand,
  mot11_i2c_key__getDriveState,
  mot11_i2c_key__startCurrentAutotuning,

  mot11_i2c_key__count,
}e_mot11_i2c_key_t;


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

  uint8_t data[9];

}u_mot11_i2c_payload_t;
#pragma pack (pop)

#define MOT11_CHANNEL_COUNT 1

//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_MOT11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal Constructor
                    HAL_MOT11           ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                    ();        
    //Modul Error Interface   
    uint8_t         getModuleErrorCount           ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

    //Spezifisch
    void            startCurrentAutotuning();  
    
  
    private:
    //Settings    
    uint8_t   deviceAddress;
    e_deviceState_t         deviceState;

    //I2C Kommunikation
    void sendDriveCommand     (const u_HAL_DATA_t DRIVE_PARAMETER);
    void requestDriveParameter();
    void sendFrame            (const u_mot11_i2c_payload_t COMMAND);
    bool waitForACK           ();
    bool waitForDriveParameter();

    Timeout to_parameterPoll; //ping timer
    Timeout to_I2C;           //max Wartezeit auf Antwort
   
    //Safety
    struct 
    {
      struct 
      {
        uint8_t count;      //counter bis error ausgegeben wird
        uint8_t countLimit; //Limit ab wann error ausgegeben wird
      }i2cError;    
    }errordetection;
    //Object handling
    struct 
    {
      IO_Interface*           p_ioObject;   
    }channels;
};
#endif