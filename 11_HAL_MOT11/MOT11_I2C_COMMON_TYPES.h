#ifndef MOT11_I2C_COMMON_TYPES_h
#define MOT11_I2C_COMMON_TYPES_h

#include "Arduino.h"
//-------------------------------------------------------------
//APPLIKATION
//-------------------------------------------------------------
#define ACK 0x06
#define NAK 0x15
const uint8_t MOT11_I2C_ADDRESSES[1] = {0xA1};

//State auf MOT11
typedef enum
{
  MOT11_DEVICE_STATE__INIT,
  MOT11_DEVICE_STATE__RUNNING,
  MOT11_DEVICE_STATE__SAFE_STATE,
  MOT11_DEVICE_STATE__AUTOTUNING,

  MOT11_DEVICE_STATE__COUNT,
}e_MOT11_DEVICE_STATE_t;

typedef enum
{
  //setter 
  MOT11_I2C_KEY__DRIVE_COMMAND,
  MOT11_I2C_KEY__GET_DRIVE_STATE,
  MOT11_I2C_KEY__START_CURRENT_AUTOTUNING,

  MOT11_I2C_KEY__COUNT,
}e_MOT11_I2C_KEY_t;


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


#endif