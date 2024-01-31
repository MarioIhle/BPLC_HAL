#include "I2C_check.h"

I2C_check::I2C_check()
{}

bool I2C_check::begin(const uint8_t ADDRESS)
{
    this->deviceAddress = ADDRESS;
    this->to_heartbeat.setInterval(5000);
    return this->checkI2CConnection();
}

bool I2C_check::checkI2CConnection()
{
    Wire.beginTransmission(this->deviceAddress);
    const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0);    
    return DEVICE_FOUND;
}

bool I2C_check::requestHeartbeat()
{
    bool heartBeatReceived = true;

    if(this->to_heartbeat.checkAndReset())
    {
        heartBeatReceived = checkI2CConnection();
    }
    return heartBeatReceived;
}