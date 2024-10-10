#include "BPLC_I2C_Nodes.h"

I2C_BPLC_Slave::I2C_BPLC_Slave()
{}
void I2C_BPLC_Slave::begin(const uint8_t ADDRESS)
{
    this->comNode.begin(ADDRESS);
}
bool I2C_BPLC_Slave::newCommandAvailable()
{
    bool NEW_COMMAND_TO_PROCESS = false;

    switch (this->comNode.newFrameReceived())
    {
        case I2C_BPLC_KEY__SLAVE_COMMAND:
            NEW_COMMAND_TO_PROCESS = true;
            this->comNode.sendACK();
            break;
        
        case I2C_BPLC_KEY__REQUEST_SLAVE_DATA:
            this->sendSlaveData();
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
    s_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->comNode.getFrame();  
    memcpy(P_PAYLOADBUFFER, NEW_FRAME.frame.data, NEW_FRAME.frameSize);

    return NEW_FRAME.frameSize;
}
void I2C_BPLC_Slave::setSlaveData(uint8_t* BUFFER, const uint8_t SIZE)
{
    if(SIZE < sizeof(this->slaveDataBuffer))
    {
        for(int i=0; i<SIZE; i++)
        {
            this->slaveDataBuffer[i] = BUFFER[i];
        }
        this->sizeOfSlaveData = SIZE;
    } 
}
void I2C_BPLC_Slave::sendSlaveData()
{    
    this->comNode.sendFrame(I2C_BPLC_KEY__SLAVE_DATA, this->slaveDataBuffer, this->sizeOfSlaveData); 
}