#include "BPLC_I2C_Nodes.h"

void I2C_BPLC_Master::begin()
{
    this->i2cNode.begin();
}
void I2C_BPLC_Master::getSlaveData(const uint8_t SLAVE_ADDRESS, const uint8_t SLAVE_PACKET, uint8_t* P_DATA_BUFFER, const uint8_t PAYLOAD_SIZE)
{       
    const uint8_t PACKET_DEFINITION[2] = {SLAVE_PACKET, PAYLOAD_SIZE};

    this->i2cNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__SET_REQUESTED_CHANNEL, &PACKET_DEFINITION[0], sizeof(PACKET_DEFINITION));
    this->i2cNode.requestFromSlave(SLAVE_ADDRESS, P_DATA_BUFFER, PAYLOAD_SIZE);
    //richtiges paket empfangen?   
}
void I2C_BPLC_Master::sendCommand(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT)
{
    this->i2cNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__SLAVE_COMMAND, P_DATA_BUFFER, BYTE_COUNT);
}