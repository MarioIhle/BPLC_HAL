#include "BPLC_I2C_Nodes.h"

//receiveCallback
s_I2C_BPLC_NODE_FRAME_t callback_inBuffer;

void receiveCallback(int howMany)
{
    for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
    {    
        callback_inBuffer.frame.data[32] = Wire.read();    
    }
    callback_inBuffer.frameSize = howMany; 
}

BPLC_I2C_NODE::BPLC_I2C_NODE()
{}

void BPLC_I2C_NODE::begin(const uint8_t ADDRESS)
{
    //I2C Kommunikation aufbauen
    if(ADDRESS != 0)
    {
        Wire.begin(ADDRESS);
    }
    else
    {
        Wire.begin();
    }    
    Wire.onReceive(receiveCallback);
}

void BPLC_I2C_NODE::sendACK()
{
    this->sendFrame(I2C_BPLC_KEY__ACK, nullptr, 1);
}

void BPLC_I2C_NODE::sendNAK()
{
    this->sendFrame(I2C_BPLC_KEY__NAK, nullptr, 1);
}

void BPLC_I2C_NODE::sendFrame(const e_I2C_BPLC_KEY_t KEY, const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
    s_I2C_BPLC_NODE_FRAME_t OUT_FRAME;

    OUT_FRAME.frameSize         = BYTE_COUNT;
    OUT_FRAME.frame.extract.key = KEY;

    if(PAYLOAD != nullptr)
    {
        for(uint8_t byte = 0; byte < BYTE_COUNT; byte++)
        {
            OUT_FRAME.frame.extract.payload[byte] = PAYLOAD[byte];
        } 
    }  

    switch (KEY)
    {
    case I2C_BPLC_KEY__REQUEST_SLAVE_DATA: 
        Wire.write(KEY);
        break;
    
    case I2C_BPLC_KEY__SLAVE_COMMAND:        
        if(PAYLOAD != nullptr)
        {
            for(uint8_t byte = 0; byte < BYTE_COUNT; byte++)
            {
                Wire.write(OUT_FRAME.frame.data[byte]);
            } 
        }        
        break;

    default:
        break;
    }   
}

e_I2C_BPLC_KEY_t BPLC_I2C_NODE::newFrameReceived()
{
    return (e_I2C_BPLC_KEY_t)callback_inBuffer.frame.extract.key;
}

s_I2C_BPLC_NODE_FRAME_t BPLC_I2C_NODE::getFrame()
{
    s_I2C_BPLC_NODE_FRAME_t NEW_FRAME = callback_inBuffer;
    memset(&callback_inBuffer, 0, sizeof(callback_inBuffer));

    return NEW_FRAME;
}
