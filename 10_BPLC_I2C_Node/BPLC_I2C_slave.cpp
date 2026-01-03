#include "BPLC_I2C_Nodes.h"

I2C_BPLC_Slave::I2C_BPLC_Slave()
{}
void I2C_BPLC_Slave::begin(const uint8_t ADDRESS)
{
    this->i2cNode.begin(ADDRESS, &this->slaveDataBuffer);
}
bool I2C_BPLC_Slave::newCommandAvailable()
{
    bool newFrameNeedsProcessing    = false;    
    const bool NEW_FRAME_RECEIVED   = this->i2cNode.newFrameReceived();   
   
    if(NEW_FRAME_RECEIVED)
    {
        const e_I2C_BPLC_KEY_t BPLC_I2C_KEY = (e_I2C_BPLC_KEY_t)this->i2cNode.getFrame().extract.i2cBplcKey; 
    
        switch(BPLC_I2C_KEY)
        {
            case I2C_BPLC_KEY__SLAVE_COMMAND:            
                newFrameNeedsProcessing = true;
                break;        

            default:
                break;
        }
    }
    return newFrameNeedsProcessing;
}
uint8_t I2C_BPLC_Slave::getCommand(uint8_t* P_PAYLOADBUFFER)
{         
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->i2cNode.getFrame();  
    memcpy(P_PAYLOADBUFFER, NEW_FRAME.extract.payload, NEW_FRAME.extract.payloadSize);

    return NEW_FRAME.extract.payloadSize;
}
bool I2C_BPLC_Slave::setSlaveData(uint8_t* BUFFER, const uint8_t SIZE)
{    
    bool sizeToBig = false;

    if(SIZE < PAYLAOD_BYTES_MAX)
    {
        memcpy(this->slaveDataBuffer.extract.payload, BUFFER, SIZE);
        this->slaveDataBuffer.extract.i2cBplcKey    = I2C_BPLC_KEY__SLAVE_DATA;
        this->slaveDataBuffer.extract.payloadSize   = SIZE;  
    }      
    else
    {
        Serial.println("SLAVE_DATA_TO_BIG");
        sizeToBig = true;
    }
    return sizeToBig;
}