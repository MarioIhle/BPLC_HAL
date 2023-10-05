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
bool    f_therIsANewMessage;
uint8_t howMany_lastTime;
bool    f_ACKRequestExpected;

uint8_t heartBeatBuffer[4];

void I2C_Slave_sendPaylaod(const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
  for(uint8_t byte = 0; byte < BYTE_COUNT; byte++)
  {
    Wire.write(PAYLOAD[byte]);
  } 
}

void requestCallback()
{
  if(f_ACKRequestExpected)
  {
    Wire.write(ACK);
    f_ACKRequestExpected = false;
  }  
  else
  {
    I2C_Slave_sendPaylaod(&heartBeatBuffer[0], 4);
  }
}

void I2C_Slave_setHeartbeatPayload(const uint8_t DIRECTION, const uint8_t SPEED, const uint8_t ERROR)
{
  heartBeatBuffer[0] = 1;
  heartBeatBuffer[1] = DIRECTION;
  heartBeatBuffer[2] = SPEED;
  heartBeatBuffer[3] = ERROR;
}

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
  Wire.onRequest(requestCallback); 
}

void I2C_Slave_sendACK()
{
  Wire.beginTransmission(I2C_MASTER_ADDRESS);
  Wire.write(ACK);
  Wire.endTransmission();
}

void I2C_Slave_sendNAK()
{
  Serial.println("SEND ACK");
  Wire.beginTransmission(I2C_MASTER_ADDRESS);
  Wire.write(NAK);
  Wire.endTransmission();
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