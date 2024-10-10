#ifndef BPLC_I2C_Nodes_h
#define BPLC_I2C_Nodes_h

#include "Arduino.h"
#include "Wire.h"
#include "SpecialFunctions.h"

//I2C Commands
#define ACK 0x06
#define NAK 0x15

typedef enum
{
  I2C_BPLC_KEY__NO_KEY,

  I2C_BPLC_KEY__REQUEST_SLAVE_DATA,
  I2C_BPLC_KEY__SLAVE_DATA,

  I2C_BPLC_KEY__SLAVE_COMMAND,  
  I2C_BPLC_KEY__ACK,
  I2C_BPLC_KEY__NAK,

}e_I2C_BPLC_KEY_t;

#pragma pack (push, 1)
typedef struct 
{   
  union 
  {
    struct 
    {
      uint8_t key;          
      uint8_t payload[31];
    }extract; 

    uint8_t data[32];
  }frame;
  
  uint8_t frameSize;

}s_I2C_BPLC_NODE_FRAME_t;
#pragma pack (pop)


class BPLC_I2C_NODE
{
  public:
            BPLC_I2C_NODE                           ();
    void    begin                                   (const uint8_t ADDRESS = 0);  
   
    e_I2C_BPLC_KEY_t        newFrameReceived        ();
    s_I2C_BPLC_NODE_FRAME_t getFrame                ();
    
    void    sendFrame                               (const e_I2C_BPLC_KEY_t KEY, const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT);  
    void    sendACK                                 ();
    void    sendNAK                                 ();
  

  private:

  bool f_ACKreceived;
  bool f_slaveDataRequested;
};


class I2C_BPLC_Master
{
  public:
            I2C_BPLC_Master   (){};
  void      begin             ();

  uint8_t   getSlaveData      (const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER); 
  bool      sendCommand       (const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT);
  

  private:

  BPLC_I2C_NODE comNode;
};

class I2C_BPLC_Slave
{
  public:
          I2C_BPLC_Slave          ();
  void    begin                   (const uint8_t ADDRESS);
 
  void    setSlaveData            (uint8_t* BUFFER, const uint8_t SIZE);
  
  bool    newCommandAvailable     (); //Wie tick, muss zyklisch aufgerufen werden
  uint8_t getCommand              (uint8_t* P_BUFFER);

  private:
  void    sendSlaveData           ();  

  uint8_t slaveDataBuffer[32];  
  uint8_t sizeOfSlaveData;

  BPLC_I2C_NODE comNode;
};
#endif