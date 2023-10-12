#include "I2C_check.h"

I2C_check::I2C_check()
{}

I2C_check::begin(const uint8_t ADDRESS)
{
    this->deviceAddress = ADDRESS;
    this->to_to_i2cCheck.setInterval(5000);
}

bool I2C_check::checkI2CConnection(const uint8_t ADDRESS)
{
    Wire.beginTransmission(ADDRESS);
    const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0);
    
    #ifdef DEBUG_I2C_Check
    Serial.print("Check connection with I2C address: 0x"); Serial.println(ADDRESS, HEX); 

    if(DEVICE_FOUND)
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
    }
    #endif
    
    return DEVICE_FOUND;
}

bool I2C_check::heartBeat()
{
    bool heartBeatReceived = true;

    if(this->to_i2cCheck.check())
    {
        heartBeatReceived = checkI2CConnection(this->deviceAddress);
        this->to_i2cCheck.reset();
    }
    return heartBeatReceived;
}