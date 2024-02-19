#ifndef HAL_MOT11_h
#define HAL_MOT11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "Wire.h"
#include "I2C_check.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_ERRORS.h"
#include "BPLC_LOG.h"
#include "HAL_interface.h"
//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
  MOT11_CARD_1_ADDRESS = 0x10,
  MOT11_CARD_2_ADDRESS = 0x11,
  MOT11_CARD_3_ADDRESS = 0x12,
  MOT11_CARD_4_ADDRESS = 0x13,
  
  MOT11_CARD_ADDRESS_COUNT = 4,
}e_MOT11_ADDRESS_t;

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
class HAL_MOT11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
    //setup
                    HAL_MOT11           (const e_MOT11_ADDRESS_t I2C_ADDRESS);
    void            init                ();
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        (){return this->getError();}  

    void            startCurrentAutotuning();  
    
  
    private:
    //Settings    
    e_MOT11_ADDRESS_t   deviceAddress;
    e_deviceState_t     deviceState;

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