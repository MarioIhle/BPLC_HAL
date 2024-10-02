#include "BPLC_I2C_Nodes.h"


//receiveCallback
uint8_t callback_inBuffer[32];
uint8_t callback_sizeOfLastMessage;

void receiveCallback(int howMany)
{
    for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
    {    
        callback_inBuffer[BYTE] = Wire.read();    
    }
    //bei >1 war es ein HB ping ohne payload
    if(howMany>1)
    {
        callback_sizeOfLastMessage     = howMany;
    }
}

BPLC_I2C_NODE::begin(const uint8_t ADDRESS, void (*p_requestCallback)(void))
{
    //I2C Kommunikation aufbauen
    this->i2cAddress = ADDRESS;
    Wire.begin(ADDRESS);
    Wire.onReceive(receiveCallback);    //Kommando das keine Antwort erwartet
    Wire.onRequest(p_requestCallback);  //Applikationsspezifische anfrage, die Antwort erwartet
}

void BPLC_I2C_NODE::sendACK()
{
    Wire.write(ACK);
}

void BPLC_I2C_NODE::sendNAK()
{
    Wire.write(NAK);
}

void BPLC_I2C_NODE::sendFrame(const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
  for(uint8_t byte = 0; byte < BYTE_COUNT; byte++)
  {
    Wire.write(PAYLOAD[byte]);
  } 
}

bool BPLC_I2C_NODE::newFrameAvailable()
{
    return (callback_sizeOfLastMessage > 0)
}

uint8_t BPLC_I2C_NODE::getSizeOfLastMessage()
{       
    return callback_sizeOfLastMessage;
}

uint8_t BPLC_I2C_NODE::getLastCommand(uint8_t* P_BUFFER)
{
    const uint8_t BYTE_COUNT    = callback_sizeOfLastMessage;
    callback_sizeOfLastMessage  = 0;

    for(uint8_t BYTE =0; BYTE< callback_sizeOfLastMessage; BYTE++)
    {
        P_BUFFER[BYTE] = this->callback_inBuffer[BYTE];
    }    
    return BYTE_COUNT
}
