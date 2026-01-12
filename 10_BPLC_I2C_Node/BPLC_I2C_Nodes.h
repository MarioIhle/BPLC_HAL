#ifndef BPLC_I2C_Nodes_h
#define BPLC_I2C_Nodes_h

#include "Arduino.h"
#include "Wire.h"
#include "SpecialFunctions.h"
#include "BPLC_logPrint.h"

typedef enum
{
  I2C_BPLC_KEY__NO_KEY,                 //Idle

  I2C_BPLC_KEY__SLAVE_COMMAND,          //Kommando von Master an Slave (z.B. Ausgang schreiben, Motor ansteuern..)  

  I2C_BPLC_KEY__SET_REQUESTED_PAYLOAD,  //Wie viele Bytes werden insgesamt angefragt  
  I2C_BPLC_KEY__REQUEST_SLAVE_DATA,         //Master hat Slavedaten angefragt (z.B. Status von Eingang versenden)
  I2C_BPLC_KEY__SLAVE_DATA                  //Slave versendet Daten an Master

}e_I2C_BPLC_KEY_t;

#define MESSAGE_HEAD        2
#define PAYLAOD_BYTES_MAX   30
#define MAX_FRAME_SIZE      32  // Mehr kann der i2c Buffer nicht (Wire.h)
#define SLAVE_DATA_PACKETS  8   //Jeder Slave hat 8 Datenpakete, die von Master angefragt werden können 

#pragma pack (push, 1)
typedef union 
{   
  struct 
  {
    uint8_t i2cBplcKey;   
    uint8_t payloadSize;       
    uint8_t payload[PAYLAOD_BYTES_MAX];
  }extract; 

  uint8_t data[MAX_FRAME_SIZE]; 

}u_I2C_BPLC_NODE_FRAME_t;
#pragma pack (pop)


class BPLC_I2C_NODE: private BPLC_logPrint
{
    public:
         BPLC_I2C_NODE      ();
    void begin              (const uint8_t NODE_ADDRESS = 0, uint8_t* p_slaveDataBuffer = nullptr);  
    //Receive Handling 
    void                    handleNewFrame            (u_I2C_BPLC_NODE_FRAME_t* p_newFrame);
    bool                    newSlaveCommandReceived   ();
    u_I2C_BPLC_NODE_FRAME_t getCommand                ();
    void                    sendFrame                 (const e_I2C_BPLC_KEY_t KEY, const uint8_t* P_PAYLOAD, const uint8_t PAYLOAD_SIZE);  
    //Request Handling  
    uint8_t   requestFromNode             (uint8_t* p_payloadBuffer, const uint8_t PAYLOAD_SIZE);
    uint8_t*  getSlaveDataPacket          (){return this->request.p_dataBuffer;} 
    uint8_t   getFirstByte                (){return this->request.firstByte;}
    uint8_t   getLastByte                 (){return this->request.lastByte;}    
    void      requestedDataSend           (){this->request.firstByte = 0; this->request.lastByte = 0;}
   
   
    private:
    uint8_t nodeAddress; //Addresse der Node egal ob Master instanz oder Slave instanz

    struct
    {
      uint8_t*  p_dataBuffer;      //pointer auf Daten, die bei Master Request versendet werden
      uint8_t   firstByte;   
      uint8_t   lastByte;  
    }request;

    struct 
    {
      bool                    f_newFrameReceived; 
      u_I2C_BPLC_NODE_FRAME_t commandToProcess;
    }command;    
  };

class I2C_BPLC_Master
{
  public:
        I2C_BPLC_Master   (){};
  void  begin             (const uint8_t NODE_ADDRESS);

  void  getSlaveData      (uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE); 
  void  sendCommand       (uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE);

  private:  
  BPLC_I2C_NODE i2cNode;
};

class I2C_BPLC_Slave
{
  public:
          I2C_BPLC_Slave          ();
  void    begin                   (const uint8_t NODE_ADDRESS);
 
  bool    setSlaveData            (uint8_t* BUFFER, const uint8_t SIZE);
  
  bool    newCommandAvailable     (); //Wie tick, muss zyklisch aufgerufen werden
  uint8_t getCommand              (uint8_t* P_BUFFER);

  private:
  BPLC_I2C_NODE i2cNode;
  uint8_t       slaveDataBuffer[256];  
};
#endif