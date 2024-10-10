#include "BPLC_I2C_Nodes.h"



I2C_BPLC_Slave::I2C_BPLC_Slave()
{}

void I2C_BPLC_Slave::begin(const uint8_t ADDRESS, void (*p_requestCallback)(void))
{
    this->comNode.begin(ADDRESS, p_requestCallback);
}

bool I2C_BPLC_Slave::thereIsANewMessage()
{
    return this->comNode.newFrameAvailable();
}

uint8_t I2C_BPLC_Slave::getLastCommand(uint8_t* P_PAYLOADBUFFER)
{         
    return this->comNode.getFrame(P_PAYLOADBUFFER);    
}

void I2C_BPLC_Slave::setResponsePayload(const uint8_t* PAYLOAD_BUFFER, const uint8_t SIZE)
{
    if(SIZE < sizeof(this->requestResponseBuffer))
    {
        for(int i=0; i<SIZE; i++)
        {
            this->requestResponseBuffer[i] = PAYLOAD_BUFFER[i];
        }
    } 
}

void I2C_BPLC_Slave::sendAckData()
{
    this->comNode.sendFrame(this->requestResponseBuffer, )
}