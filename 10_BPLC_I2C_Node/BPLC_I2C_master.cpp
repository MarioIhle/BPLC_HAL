#include "BPLC_I2C_Nodes.h"

void I2C_BPLC_Master::begin()
{
    this->i2cNode.begin();
}
bool I2C_BPLC_Master::getSlaveData(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE)
{    
    const uint8_t BYTES_REQUESTED = (PAYLOAD_SIZE + MESSAGE_HEAD);   //PAYLOAD_SIZE + i2cBplcKey + palyoadSize
    
    u_I2C_BPLC_NODE_FRAME_t slaveData;
    memset(&slaveData, 0, sizeof(slaveData));
    uint8_t inBytes = 0;
    Wire.requestFrom(SLAVE_ADDRESS, BYTES_REQUESTED, false);       
    while(Wire.available())
    {      
        slaveData.data[inBytes] = Wire.read(); 
        Serial.print(slaveData.data[inBytes]);
        inBytes++;
    }        
    memcpy(P_DATA_BUFFER, slaveData.extract.payload, PAYLOAD_SIZE);  //<-- das funktioniert??
    
    return (slaveData.extract.i2cBplcKey == I2C_BPLC_KEY__SLAVE_DATA);
}
void I2C_BPLC_Master::sendCommand(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT)
{
    this->i2cNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__SLAVE_COMMAND, P_DATA_BUFFER, BYTE_COUNT);
}