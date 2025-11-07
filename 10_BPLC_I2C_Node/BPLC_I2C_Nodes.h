#ifndef BPLC_I2C_Nodes_h
#define BPLC_I2C_Nodes_h

#include "Arduino.h"
#include "Wire.h"
#include "SpecialFunctions.h"


typedef enum
{
  I2C_BPLC_KEY__NO_KEY,

  I2C_BPLC_KEY__REQUEST_SLAVE_DATA,
  I2C_BPLC_KEY__SLAVE_DATA,

  I2C_BPLC_KEY__SLAVE_COMMAND,  

}e_I2C_BPLC_KEY_t;


#define PAYLAOD_BYTES_MAX 128
#define MESSAGE_HEAD      2

#pragma pack (push, 1)
typedef union 
{   
  struct 
  {
    uint8_t i2cBplcKey;   
    uint8_t payloadSize;       
    uint8_t payload[PAYLAOD_BYTES_MAX];
  }extract; 

  uint8_t data[PAYLAOD_BYTES_MAX + MESSAGE_HEAD]; 

}u_I2C_BPLC_NODE_FRAME_t;
#pragma pack (pop)


class BPLC_I2C_NODE
{
  public:
                            BPLC_I2C_NODE     ();
    void                    begin             (const uint8_t ADDRESS = 0, u_I2C_BPLC_NODE_FRAME_t* p_cb_request = nullptr);  
   
    e_I2C_BPLC_KEY_t        newFrameReceived  ();
    u_I2C_BPLC_NODE_FRAME_t getFrame          ();
    
    void                    sendFrame         (const uint8_t DESTINATION_ADDRESS, const e_I2C_BPLC_KEY_t KEY, const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT);  
    
  private:

};

class I2C_BPLC_Master
{
  public:
        I2C_BPLC_Master   (){};
  void  begin             ();

  bool  getSlaveData      (const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT); 
  void  sendCommand       (const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT);
  

  private:

  BPLC_I2C_NODE i2cNode;
};

class I2C_BPLC_Slave
{
  public:
          I2C_BPLC_Slave          ();
  void    begin                   (const uint8_t ADDRESS);
 
  bool    setSlaveData            (uint8_t* BUFFER, const uint8_t SIZE);
  
  bool    newCommandAvailable     (); //Wie tick, muss zyklisch aufgerufen werden
  uint8_t getCommand              (uint8_t* P_BUFFER);

  private:

  u_I2C_BPLC_NODE_FRAME_t slaveDataBuffer;  

  BPLC_I2C_NODE i2cNode;
};
#endif