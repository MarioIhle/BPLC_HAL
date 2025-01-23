#include "BPLC_I2C_Nodes.h"

I2C_BPLC_Slave::I2C_BPLC_Slave()
{}
void I2C_BPLC_Slave::begin(const uint8_t ADDRESS)
{
    this->i2cNode.begin(ADDRESS, &this->slaveDataBuffer);
}
bool I2C_BPLC_Slave::newCommandAvailable()
{
    bool NEW_COMMAND_TO_PROCESS = false;
    const e_I2C_BPLC_KEY_t BPLC_I2C_KEY  = this->i2cNode.newFrameReceived();

    switch(BPLC_I2C_KEY)
    {
        case I2C_BPLC_KEY__SLAVE_COMMAND:
            NEW_COMMAND_TO_PROCESS = true;
            this->i2cNode.sendACK(0);
            break;        

        default:
        case I2C_BPLC_KEY__ACK:
        case I2C_BPLC_KEY__NAK:
        case I2C_BPLC_KEY__SLAVE_DATA: 
            break;
    }

    return NEW_COMMAND_TO_PROCESS;
}
uint8_t I2C_BPLC_Slave::getCommand(uint8_t* P_PAYLOADBUFFER)
{         
    s_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->i2cNode.getFrame();  
    memcpy(P_PAYLOADBUFFER, NEW_FRAME.frame.data, NEW_FRAME.frameSize);

    return NEW_FRAME.frameSize;
}
void I2C_BPLC_Slave::setSlaveData(uint8_t* BUFFER, const uint8_t SIZE)
{    
    memcpy(this->slaveDataBuffer.frame.extract.payload, BUFFER, SIZE);
    this->slaveDataBuffer.frame.extract.i2cBplcKey  = I2C_BPLC_KEY__SLAVE_DATA;
    this->slaveDataBuffer.frameSize                 = SIZE;    
}