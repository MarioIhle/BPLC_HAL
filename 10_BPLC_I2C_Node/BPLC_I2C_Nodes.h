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

  I2C_BPLC_KEY__SET_REQUESTED_CHANNEL,  //Channel und Größe des Erwarteten Datenpakets der an Master versendet wird, bei Anfrage von Daten
  I2C_BPLC_KEY__REQUEST_SLAVE_DATA,     //Master hat Slavedaten angefragt (z.B. Status von Eingang versenden)
  I2C_BPLC_KEY__SLAVE_DATA              //Slave versendet Daten an Master

}e_I2C_BPLC_KEY_t;

typedef struct
{
  uint8_t index;
  uint8_t payloadSize;
}s_slaveDataPaketDefinition_t;

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
    void begin              (const uint8_t ADDRESS = 0);  
    void setPacketPointer   (const uint8_t PACKET, u_I2C_BPLC_NODE_FRAME_t* p_dataPacket);
    //Receive Handling 
    void                    handleNewFrame            (u_I2C_BPLC_NODE_FRAME_t* p_newFrame);
    bool                    newSlaveCommandReceived   ();
    u_I2C_BPLC_NODE_FRAME_t getFrame                  ();
    void                    sendFrame                 (const uint8_t DESTINATION_ADDRESS, const e_I2C_BPLC_KEY_t KEY, const uint8_t* P_PAYLOAD, const uint8_t PAYLOAD_SIZE);  
    //Request Handling
    void                          requestFromSlave          (const uint8_t SLAVE_ADDRESS, uint8_t* p_payloadBuffer, const uint8_t PAYLOAD_SIZE);
    u_I2C_BPLC_NODE_FRAME_t*      getSlaveDataBuffer        (const uint8_t PACKET){return this->request.p_slaveDataBuffer[PACKET];} 
    s_slaveDataPaketDefinition_t  getSlaveDataDefinition    (){return this->request.requestedData;}
    

    private:
    struct
    {
      u_I2C_BPLC_NODE_FRAME_t*      p_slaveDataBuffer[SLAVE_DATA_PACKETS];      //pointer auf Daten, die bei Master Request versendet werden
      s_slaveDataPaketDefinition_t  requestedData;
    }request;

    struct 
    {
      bool f_newFrameReceived; 
      u_I2C_BPLC_NODE_FRAME_t frame;
    }command;    
  };

class I2C_BPLC_Master
{
  public:
        I2C_BPLC_Master   (){};
  void  begin             ();

  void  getSlaveData          (const uint8_t SLAVE_ADDRESS,  const uint8_t SLAVE_PACKET, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT); 
  void  sendCommand           (const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT);

  private:  
  BPLC_I2C_NODE i2cNode;
};

class I2C_BPLC_Slave
{
  public:
          I2C_BPLC_Slave          ();
  void    begin                   (const uint8_t ADDRESS);
 
  bool    setSlaveData            (const uint8_t DATA_PACKET, uint8_t* BUFFER, const uint8_t SIZE);
  
  bool    newCommandAvailable     (); //Wie tick, muss zyklisch aufgerufen werden
  uint8_t getCommand              (uint8_t* P_BUFFER);

  private:
  BPLC_I2C_NODE i2cNode;

  u_I2C_BPLC_NODE_FRAME_t slaveDataPackets[SLAVE_DATA_PACKETS];  
};
#endif