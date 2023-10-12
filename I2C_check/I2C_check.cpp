#include "I2C_check.h"

I2C_check::I2C_check()
{}

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