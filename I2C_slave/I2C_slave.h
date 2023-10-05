#ifndef I2C_slave_h
#define I2C_slave_h

#include "Arduino.h"
#include "Wire.h"


//I2C 
#define I2C_MASTER_ADDRESS 0x00
//I2C Commands
#define ACK 0x06
#define NAK 0x15


uint8_t inBuffer[32];
uint8_t I2C_adress;
bool f_therIsANewMessage;
uint8_t howMany_lastTime;

void receiveCallback(int howMany)
{
  //Serial.print("hwoMany: "); Serial.println(howMany);

  for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
  {    
    //Serial.println(Wire.peek());
    inBuffer[BYTE] = Wire.read();    
  }   
  f_therIsANewMessage = true;

  howMany_lastTime=howMany;
}

void I2C_Slave_begin(const uint8_t ADDRESS)
{
  //I2C Kommunikation aufbauen
  I2C_adress = ADDRESS;
  Wire.begin(ADDRESS);
  Wire.onReceive(receiveCallback);
}

void I2C_Slave_sendPaylaod(const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
  for(uint8_t byte = 0; byte < BYTE_COUNT; byte++)
  {
    Wire.write(PAYLOAD[byte]);
  } 
}

void I2C_Slave_sendACK()
{
  Wire.write(ACK);
}

void I2C_Slave_sendNAK()
{
  Wire.write(NAK);
}

bool I2C_Slave_isThereANewMessage()
{
  return f_therIsANewMessage;
}

uint8_t I2C_Slave_getPayload(uint8_t* P_PAYLOADBUFFER)
{
  for(uint8_t BYTE =0; BYTE< sizeof(inBuffer); BYTE++)
  {
    P_PAYLOADBUFFER[BYTE] = inBuffer[BYTE];
  }
  f_therIsANewMessage = false;
  return howMany_lastTime;
}

#endif