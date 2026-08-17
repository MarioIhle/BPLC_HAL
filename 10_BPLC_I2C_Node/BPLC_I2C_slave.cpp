#include "BPLC_I2C_Nodes.h"

I2C_BPLC_Slave::I2C_BPLC_Slave()
{}
void I2C_BPLC_Slave::begin(const uint8_t ADDRESS)
{
    this->i2cNode.begin(ADDRESS, &this->slaveDataBuffer[0]);
}
bool I2C_BPLC_Slave::newCommandAvailable()
{
    return this->i2cNode.newSlaveCommandReceived();   
}
uint8_t I2C_BPLC_Slave::getCommand(uint8_t* P_PAYLOADBUFFER, const uint8_t BUFFER_SIZE)
{         
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->i2cNode.getCommand();  
    const uint8_t BYTES_TO_COPY = min(NEW_FRAME.extract.payloadSize, BUFFER_SIZE);
    if(P_PAYLOADBUFFER != nullptr)
    {
        memcpy(P_PAYLOADBUFFER, NEW_FRAME.extract.payload, BYTES_TO_COPY);
    }

    return BYTES_TO_COPY;
}
bool I2C_BPLC_Slave::setSlaveData(uint8_t* BUFFER, const uint8_t SIZE)
{    
    bool error = false;

    if(SIZE > sizeof(this->slaveDataBuffer))
    {
        Serial.println("slave data too big");
        error = true;        
    } 
    else
    {
        memset(&this->slaveDataBuffer[0], 0, sizeof(this->slaveDataBuffer));
        memcpy(&this->slaveDataBuffer[0], BUFFER, SIZE);
        
        /*
        Serial.println("Set Slave packet: ");
        for(uint8_t i = 0; i < SIZE; i++)
        {
            Serial.print(this->slaveDataBuffer[i]);
        }
        Serial.println("");
        */        
    }
    return error;
}