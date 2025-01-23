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

//requestCallback
s_I2C_BPLC_NODE_FRAME_t* p_callback_outBuffer;  //pointer auf Daten, die bei Master Request versendet werden
void requestCallback()
{    
    Wire.write(p_callback_outBuffer->frame.data, (p_callback_outBuffer->frameSize + 1));  //PAYLOAD_SIZE + i2cBplcKey  

    /*
    for(uint8_t i = 0; i< p_callback_outBuffer->frameSize; i++)
    {
        Serial.print(p_callback_outBuffer->frame.data[i]);
    } 
    */   
}



BPLC_I2C_NODE::BPLC_I2C_NODE()
{}
void BPLC_I2C_NODE::begin(const uint8_t ADDRESS, s_I2C_BPLC_NODE_FRAME_t* p_cb_request)
{
    //I2C Kommunikation aufbauen
    if(ADDRESS != 0)
    {
        Wire.begin(ADDRESS);
    }
    //I2C taktfrequenz auf 400kHz erhöhen
    Wire.setClock(400000);
   
  
    Wire.onReceive(receiveCallback);
    Wire.onRequest(requestCallback);
    p_callback_outBuffer = p_cb_request;
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

    OUT_FRAME.frame.extract.i2cBplcKey = (uint8_t)KEY;

    if(PAYLOAD != nullptr)
    {        
        memcpy(OUT_FRAME.frame.extract.payload, PAYLOAD, BYTE_COUNT);
    }   

    //Anwort Frame darf kein Wire.begin() haben
    switch (KEY)
    {   
        default:
        case I2C_BPLC_KEY__NO_KEY:
            //error
            break;

        case I2C_BPLC_KEY__REQUEST_SLAVE_DATA:
        case I2C_BPLC_KEY__SLAVE_COMMAND:
            Wire.beginTransmission(DESTINATION_ADDRESS);
            Wire.write(OUT_FRAME.frame.data, 1+BYTE_COUNT);
            Serial.write(OUT_FRAME.frame.data, 1+BYTE_COUNT);
            Wire.endTransmission(true); 
            break;

        case I2C_BPLC_KEY__ACK:
        case I2C_BPLC_KEY__NAK:
        case I2C_BPLC_KEY__SLAVE_DATA:
            Wire.write(OUT_FRAME.frame.data, 1+BYTE_COUNT);
            Serial.write(OUT_FRAME.frame.data, 1+BYTE_COUNT);
            break;
    }            
}
e_I2C_BPLC_KEY_t BPLC_I2C_NODE::newFrameReceived()
{
    return (e_I2C_BPLC_KEY_t)callback_inBuffer.frame.extract.i2cBplcKey;
}
s_I2C_BPLC_NODE_FRAME_t BPLC_I2C_NODE::getFrame()
{
    s_I2C_BPLC_NODE_FRAME_t NEW_FRAME = callback_inBuffer;
    memset(&callback_inBuffer, 0, sizeof(callback_inBuffer));

    return NEW_FRAME;
}