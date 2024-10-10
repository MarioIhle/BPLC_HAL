#ifndef BPLC_I2C_Nodes_h
#define BPLC_I2C_Nodes_h

#include "Arduino.h"
#include "Wire.h"


//I2C 
#define I2C_MASTER_ADDRESS 0x00
//I2C Commands
#define ACK 0x06
#define NAK 0x15

class BPLC_I2C_NODE
{
  public:
          BPLC_I2C_NODE       ();
    void  setAddress          (const uint8_t ADDRESS)             {this->address = ADDRESS;}
    void  setReceiveCallback  (void (*P_RECEIVE_CALLBACK)(void))  {this->p_receiveCallback = P_RECEIVE_CALLBACK;}

    bool    newFrameAvailable     ();
    void    getFrame              (uint8_t* P_BUFFER);

    void    requestPayload             ();
    void    sendFrame                (const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT);  
    void    sendACK                    ();
    void    sendNAK                    ();
  
  
  private:
  uint8_t address;
  void*   p_receiveCallback;
};


class I2C_BPLC_Master
{
  public:
        I2C_BPLC_Master ();
  void  begin           ();
  bool  requestACK      ();
  void  sendCommand     ();
  

  private:
  BPLC_I2C_NODE comNode;

};

class I2C_BPLC_Slave
{
  public:
          I2C_BPLC_Slave          ();
  void    begin                   (const uint8_t ADDRESS, void (*p_requestCallback)(void));

  bool    thereIsANewMessage      (); 
  void    setResponsePayload      (const uint8_t* PAYLOAD_BUFFER, const uint8_t SIZE);
  uint8_t getLastCommand          (uint8_t* P_BUFFER);
  
  void    setSizeOfResponse       (const uint8_t RESPONSE_SIZE){this->sizeOfResponse = RESPONSE_SIZE;}

  private:
  void    sendAckData             ();
  BPLC_I2C_NODE comNode;

  uint8_t requestResponseBuffer[32];  //Antwort die bei nächster eingehender Request versendet wird
  uint8_t sizeOfResponse;

  uint8_t inBuffer[32];
  bool    f_thereIsANewMessage;
};
#endif