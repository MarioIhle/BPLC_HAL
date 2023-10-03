#ifndef I2C_slave_h
#define I2C_slave_h

#include "Arduino.h"
#include "Wire.h"


//I2C 
#define I2C_MASTER_ADDRESS 0x00
//I2C Commands
#define ACK 0x06
#define NAK 0x15

class I2C_Slave
{
  public:
  I2C_Slave();
  I2C_Slave(uint8_t ADDRESS);
  void  begin(uint8_t ADDRESS);

  void  tick();

  bool  isThereANewMessage();
  void  getPayload(uint8_t* P_PAYLOADBUFFER);

  private:
  void sendACK();
  void sendNAK();
  void reviceCallback();

  uint8_t I2C_adress;
  uint8_t inBuffer[16];

  bool f_therIsANewMessage;
};
#endif