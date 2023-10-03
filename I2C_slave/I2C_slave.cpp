#include "I2C_slave.h"

I2C_Slave::I2C_Slave()
{}

I2C_Slave::I2C_Slave(const uint8_t ADDRESS)
{
  this->I2C_adress = ADDRESS;
}

void I2C_Slave::begin(const uint8_t ADDRESS)
{
  //I2C Kommunikation aufbauen
  this->I2C_adress = ADDRESS;
  Wire.begin(ADDRESS);
}

void I2C_Slave::tick()
{
  if(Wire.available()>0)
  {
    Serial.println("newMassage available");
    this->f_therIsANewMessage = true;
    this->reviceCallback();
  }
}

void I2C_Slave::reviceCallback()
{
  for(uint8_t BYTE = 0; BYTE < 16; BYTE++)
  {
    if(Wire.available()) //möglicherweise Überflüssig?
    {
      this->inBuffer[BYTE] = Wire.read();
    }
    else
    {
      break;
    }
  }  
}

void I2C_Slave::sendACK()
{
  Wire.beginTransmission(I2C_MASTER_ADDRESS);
  Wire.write(ACK);
  Wire.endTransmission();
}

void I2C_Slave::sendNAK()
{
  Wire.beginTransmission(I2C_MASTER_ADDRESS);
  Wire.write(NAK);
  Wire.endTransmission();
}

bool I2C_Slave::isThereANewMessage()
{
  return this->f_therIsANewMessage;
}

void I2C_Slave::getPayload(uint8_t* P_PAYLOADBUFFER)
{
  for(uint8_t BYTE =0; BYTE< sizeof(this->inBuffer); BYTE++)
  {
    P_PAYLOADBUFFER[BYTE] = this->inBuffer[BYTE];
  }
  this->f_therIsANewMessage = false;
}