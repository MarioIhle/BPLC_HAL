#include "BPLC_I2C_Nodes.h"

void I2C_BPLC_Master::begin()
{
    this->i2cNode.begin();
}
bool I2C_BPLC_Master::getSlaveData(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE)
{    
    Wire.requestFrom(SLAVE_ADDRESS, (PAYLOAD_SIZE + 1));    //PAYLOAD_SIZE + i2cBplcKey
  
    s_I2C_BPLC_NODE_FRAME_t slaveData;
    memset(&slaveData, 0, sizeof(slaveData));

    while(Wire.available())
    {      
        slaveData.frame.data[slaveData.frameSize] = Wire.read(); 
        slaveData.frameSize++;
    }        
    memcpy(P_DATA_BUFFER, slaveData.frame.extract.payload, PAYLOAD_SIZE);  
    
    return (slaveData.frame.extract.i2cBplcKey == I2C_BPLC_KEY__SLAVE_DATA);
}
bool I2C_BPLC_Master::sendCommand(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT)
{
    this->i2cNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__SLAVE_COMMAND, P_DATA_BUFFER, BYTE_COUNT);
    
    Timeout to_waitForACK(50);

    while (to_waitForACK.check() == false)
    {        
        if (I2C_BPLC_KEY__ACK == this->i2cNode.newFrameReceived())
        {
            return true;
        }
        else
        {            
            //error?
        }        
    }   
    return false; 
}