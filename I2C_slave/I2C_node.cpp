#include "BPLC_I2C_Nodes.h"

//receiveCallback
s_I2C_BPLC_NODE_FRAME_t callback_inBuffer;
void receiveCallback(int howMany)
{
    while(Wire.available())
    {
        for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
        {    
            callback_inBuffer.frame.data[BYTE] = Wire.read();    
        }
    }    
    callback_inBuffer.frameSize = (uint8_t)howMany; 
}
bool f_dataRequested;
void requestCallback()
{
    Wire.write(5);
    f_dataRequested = true;
}
bool BPLC_I2C_NODE::masterOnRevceive()
{
    bool MASTER_IS_READY = f_dataRequested;
    f_dataRequested = false;
    return MASTER_IS_READY;
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
    Wire.onRequest(requestCallback);
}
void BPLC_I2C_NODE::sendACK(const uint8_t DESTINATION_ADDRESS)
{
    this->sendFrame(DESTINATION_ADDRESS, I2C_BPLC_KEY__ACK, nullptr, 1);
}
void BPLC_I2C_NODE::sendNAK(const uint8_t DESTINATION_ADDRESS)
{
    this->sendFrame(DESTINATION_ADDRESS, I2C_BPLC_KEY__NAK, nullptr, 1);
}
void BPLC_I2C_NODE::sendFrame(const uint8_t DESTINATION_ADDRESS, const e_I2C_BPLC_KEY_t KEY, const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
    s_I2C_BPLC_NODE_FRAME_t OUT_FRAME;
    memset(&OUT_FRAME, 0, sizeof(OUT_FRAME));

    OUT_FRAME.frame.extract.key = KEY;

    if(PAYLOAD != nullptr)
    {        
        memcpy(OUT_FRAME.frame.extract.payload, PAYLOAD, BYTE_COUNT);
    }      
    Wire.beginTransmission(DESTINATION_ADDRESS);
    Wire.write(OUT_FRAME.frame.data, 1+BYTE_COUNT);
    Wire.endTransmission(true);          
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