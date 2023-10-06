#ifndef I2C_slave_h
#define I2C_slave_h

#include "Arduino.h"
#include "Wire.h"


//I2C 
#define I2C_MASTER_ADDRESS 0x00
//I2C Commands
#define ACK 0x06
#define NAK 0x15

class I2C_BPLC_Slave
{
  public:
  I2C_BPLC_Slave();
  void begin(const uint8_t ADDRESS, void (*p_requestCallback)(void));

  bool    thereIsANewMessage(); 
  void    setNextRequestPayload(const uint8_t* PAYLOAD_BUFFER, const uint8_t SIZE);
  uint8_t getLastMessagePayload(uint8_t* P_BUFFER);

  void sendPayload(const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT);  
  void sendACK();
  void sendNAK();

  private:
  uint8_t requestResponseBuffer[32];  //Antwort die bei nächster eingehender Request versendet wird
  uint8_t i2cAddress;

  uint8_t inBuffer[32];
  bool    f_thereIsANewMessage;
  uint8_t sizeOfLastMessage; 
};
#endif