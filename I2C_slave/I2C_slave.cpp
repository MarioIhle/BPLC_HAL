#include "I2C_slave.h"

//receiveCallback
uint8_t callback_inBuffer[32];
bool    callback_f_thereIsANewMessage;
uint8_t callback_sizeOfLastMessage;

void receiveCallback(int howMany)
{
    for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
    {    
        //Serial.println(Wire.peek());
        callback_inBuffer[BYTE] = Wire.read();    
    }
    //bei >1 war es ein HB ping ohne payload
    if(howMany>1)
    {
        callback_f_thereIsANewMessage  = true;
        callback_sizeOfLastMessage     = howMany;
    }
}

I2C_BPLC_Slave::I2C_BPLC_Slave()
{}

void I2C_BPLC_Slave::begin(const uint8_t ADDRESS, void (*p_requestCallback)(void))
{
    //I2C Kommunikation aufbauen
    this->i2cAddress = ADDRESS;
    Wire.begin(ADDRESS);
    Wire.onReceive(receiveCallback);    //Kommando das keine Antwort erwartet
    Wire.onRequest(p_requestCallback);  //Applikationsspezifische anfrage, die Antwort erwartet
}

bool I2C_BPLC_Slave::thereIsANewMessage()
{
    for(uint8_t BYTE = 0; BYTE < callback_sizeOfLastMessage; BYTE++)
    {    
        this->inBuffer[BYTE] = callback_inBuffer[BYTE];    
    }
    this->f_thereIsANewMessage  = callback_f_thereIsANewMessage;
    this->sizeOfLastMessage     = callback_sizeOfLastMessage;

    //Für nächstes Event initialisieren
    callback_f_thereIsANewMessage   = false;
    callback_sizeOfLastMessage      = 0;

    return this->f_thereIsANewMessage;
}

uint8_t I2C_BPLC_Slave::getLastMessagePayload(uint8_t* P_PAYLOADBUFFER)
{
    const uint8_t MESSAGE_SIZE = this->sizeOfLastMessage;

    for(uint8_t BYTE =0; BYTE< MESSAGE_SIZE; BYTE++)
    {
        P_PAYLOADBUFFER[BYTE] = inBuffer[BYTE];
    }

    this->f_thereIsANewMessage = false;

    return MESSAGE_SIZE;
}

void I2C_BPLC_Slave::setNextRequestPayload(const uint8_t* PAYLOAD_BUFFER, const uint8_t SIZE)
{
    if(SIZE < sizeof(this->requestResponseBuffer))
    {
        for(int i=0; i<SIZE; i++)
        {
            this->requestResponseBuffer[i] = PAYLOAD_BUFFER[i];
        }
    }
    else
    {
        Serial.println("Paylaod too big for buffer");
    }  
}

void I2C_BPLC_Slave::sendACK()
{
    Wire.write(ACK);
}

void I2C_BPLC_Slave::sendNAK()
{
    Wire.write(NAK);
}

void I2C_BPLC_Slave::sendPayload(const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
  for(uint8_t byte = 0; byte < BYTE_COUNT; byte++)
  {
    Wire.write(PAYLOAD[byte]);
  } 
}