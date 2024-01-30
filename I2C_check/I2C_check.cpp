#include "I2C_check.h"

I2C_check::I2C_check()
{}

bool I2C_check::begin(const uint8_t ADDRESS)
{
    this->deviceAddress = ADDRESS;
    this->to_heartbeat.setInterval(5000);
    const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0);
    return DEVICE_FOUND;
}

bool I2C_check::checkI2CConnection()
{
    Wire.beginTransmission(this->deviceAddress);
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

bool I2C_check::requestHeartbeat()
{
    bool heartBeatReceived = true;

    if(this->to_heartbeat.check())
    {
        heartBeatReceived = checkI2CConnection();
        this->to_heartbeat.reset();
    }
    return heartBeatReceived;
}