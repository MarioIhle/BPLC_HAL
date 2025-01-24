#include "BPLC_I2C_Nodes.h"

void I2C_BPLC_Master::begin()
{
    this->i2cNode.begin();
}
bool I2C_BPLC_Master::getSlaveData(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE)
{    
    Wire.requestFrom(SLAVE_ADDRESS, (PAYLOAD_SIZE + 1));    //PAYLOAD_SIZE + i2cBplcKey
  
    u_I2C_BPLC_NODE_FRAME_t slaveData;
    memset(&slaveData, 0, sizeof(slaveData));
    uint8_t inBytes = 0;

    while(Wire.available())
    {      
        slaveData.data[inBytes] = Wire.read(); 
        inBytes++;
    }        
    memcpy(P_DATA_BUFFER, slaveData.extract.payload, PAYLOAD_SIZE);  
    
    return (slaveData.extract.i2cBplcKey == I2C_BPLC_KEY__SLAVE_DATA);
}
void I2C_BPLC_Master::sendCommand(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT)
{
    this->i2cNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__SLAVE_COMMAND, P_DATA_BUFFER, BYTE_COUNT);
}