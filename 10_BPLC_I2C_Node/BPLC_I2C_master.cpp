#include "BPLC_I2C_Nodes.h"

void I2C_BPLC_Master::begin(const uint8_t NODE_ADDRESS)
{
    this->i2cNode.begin(NODE_ADDRESS);
}
void I2C_BPLC_Master::getSlaveData(uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE)
{       
    const uint8_t BYTES_RECEIVED = this->i2cNode.requestFromNode(P_DATA_BUFFER, PAYLOAD_SIZE);
    //richtiges paket empfangen?   
    if(BYTES_RECEIVED == PAYLOAD_SIZE)
    {
        //Serial.println("alle bytes erfolgreich empfangen!");
    }
}
void I2C_BPLC_Master::sendCommand(uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE)
{
    this->i2cNode.sendFrame(I2C_BPLC_KEY__SLAVE_COMMAND, P_DATA_BUFFER, PAYLOAD_SIZE);
}