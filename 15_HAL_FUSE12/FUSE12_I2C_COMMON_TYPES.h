#ifndef FUSE12_I2C_COMMON_TYPES_h
#define FUSE12_I2C_COMMON_TYPES_h


#include "Arduino.h"
#include "I2C_slave.h"
//-------------------------------------------------------------
//APPLIKATION
//-------------------------------------------------------------
typedef enum
{
  FUSE12_I2C_KEY__SET_OUTPUT,
  FUSE12_I2C_KEY__GET_FUSE_STATES_AND_CURRENT,

  FUSE12_I2C_KEY__COUNT,
}e_FUSE11_I2C_KEY_t;


#pragma pack (push, 1)
typedef union 
{   
  struct 
  {
    uint8_t key;        //1 byte

    struct 
    {
      bool F1;
      bool F2;
      bool F3;
      bool F4;
    }fuseState;         //1 byte

    struct 
    {
      bool O1;
      bool O2;
      bool O3;
      bool O4;
    }outputStates;      //1 byte

    float current;      //4 bytes
  }extract; 

  uint8_t data[sizeof(extract)];

}u_fuse12_i2c_payload_t;
#pragma pack (pop)


#endif