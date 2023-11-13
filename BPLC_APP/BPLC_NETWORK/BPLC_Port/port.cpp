#include "BertaPorts.h"

//---------------------------------------------------------------------------------------
//BERTANET EVENTPORT KLASSE
//---------------------------------------------------------------------------------------
//STANDART CONSTRUCTOR
BertaPort::BertaPort	()
{}      
//EVENTPORT CONSTRUCTOR
BertaPort::BertaPort(e_PortType_t TYPE, uint8_t PORT_INDEX)
{
    this->type                      = TYPE;
    this->index                     = PORT_INDEX;
    this->f_newPortDataReceived     = false;
    this->f_newPortDataToSend       = false;
    this->f_portDataRequested       = false;
    memset(&this->payload, 0, sizeof(u_Payload_t));
}

//PUSHPORT CONSTRUCTOR
BertaPort::BertaPort(e_PortType_t TYPE, uint8_t PORT_INDEX, uint32_t PUSH_INTERVALL)
{
    this->type                      = TYPE;
    this->index                     = PORT_INDEX;
    this->f_newPortDataReceived     = false;
    this->f_newPortDataToSend       = false;
    this->f_portDataRequested       = false;
    this->to_push.setInterval(PUSH_INTERVALL);
    memset(&this->payload, 0, sizeof(u_Payload_t));
}

//---------------------------------------------------------------------------------------
//HANDLING
uint8_t	BertaPort::getIndex()
{
    return this->index;
}

e_PortType_t BertaPort::getType()
{
    return this->type;
}


void BertaPort::pushNow()
{
    this->to_push.now();
}

bool  BertaPort::newPortDataAvailable()
{
    return this->f_newPortDataReceived;
}

bool BertaPort::isNewPortDataAvailableToSend()
{  
    bool newPortDataAvailable = false;

    switch(this->type)
    {
        case eventPort:
            newPortDataAvailable = this->f_newPortDataToSend;
        break;

        case pushPort:
            newPortDataAvailable = (bool)(this->to_push.check());
        break;

        case readOnlyPort:
            newPortDataAvailable = false;
        break; 
    }
    return newPortDataAvailable;
}
void BertaPort::portDataStoredInRequestBuffer()
{
    switch(this->type)
    {
        case eventPort:
            this->f_newPortDataToSend = false;
        break;        

        case pushPort:
            this->to_push.reset();
        break;

        case readOnlyPort:
             this->f_newPortDataToSend = false;;
        break; 
    }   
}

//Daten von Master abfragen
void BertaPort::read()
{
    this->f_portDataRequested = true;
}

bool BertaPort::isPortDataReqeuested()
{
    return this->f_portDataRequested;
}   

void BertaPort::requestStoredInRequestBuffer()
{
    this->f_portDataRequested = false;
}



uint8_t* BertaPort::getDataLocation()
{
    return &this->payload.DATA[0];
}


//---------------------------------------------------------------------------------------
//SETTER
//Wird von Network genutzt um empfangene Daten zu speichern
void BertaPort::setPayload(u_Payload_t PAYLOAD)
{
    this->payload = PAYLOAD;
    this->f_newPortDataReceived = true;   
}
//______________________________________________________________________________________
void BertaPort::setBool(bool PAYLOAD)
{
    if(PAYLOAD != this->payload.BOOL)
    {
        this->payload.BOOL = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }    
}    
//______________________________________________________________________________________
void BertaPort::setByte(uint8_t PAYLOAD)
{    
    if(PAYLOAD != this->payload.BYTE)
    {
        this->payload.BYTE = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }
}
//______________________________________________________________________________________
void BertaPort::setInt(int PAYLOAD)
{    
    if(PAYLOAD != this->payload.INT)
    {
        this->payload.INT = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }    
}
//______________________________________________________________________________________
void BertaPort::setFloat(float PAYLOAD)
{    
    if(PAYLOAD != this->payload.FLOAT)
    {
        this->payload.FLOAT = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }
}
//______________________________________________________________________________________
void BertaPort::setRgb(s_RGB_t PAYLOAD)
{    
    if(PAYLOAD.red != this->payload.RGB.red || PAYLOAD.green != this->payload.RGB.green || PAYLOAD.blue != this->payload.RGB.blue)
    {
        this->payload.RGB = PAYLOAD;
        this->f_newPortDataToSend = true;    
    } 
}
//______________________________________________________________________________________
void BertaPort::setData(uint8_t BYTE, uint8_t PAYLOAD)
{    
    if(PAYLOAD != this->payload.DATA[BYTE])
    {
        this->payload.DATA[BYTE] = PAYLOAD;  
        this->f_newPortDataToSend = true;    
    }    
}

//---------------------------------------------------------------------------------------
//GETTER
//Wird von Network genutzt um Daten zum Senden zu lesen
u_Payload_t BertaPort::getPayload()
{  
    return this->payload;         
}

bool BertaPort::getBool()
{
    this->f_newPortDataReceived = false;
    return this->payload.BOOL;        
}

uint8_t BertaPort::getByte()
{
    this->f_newPortDataReceived = false;
    return this->payload.BYTE;        
}

int BertaPort::getInt()
{
    this->f_newPortDataReceived = false;
    return this->payload.INT;        
}

float BertaPort::getFloat()
{
    this->f_newPortDataReceived = false;
    return this->payload.FLOAT;        
}

s_RGB_t BertaPort::getRgb()
{
    this->f_newPortDataReceived = false;
    return this->payload.RGB;        
}

uint8_t BertaPort::getData(uint8_t BYTE)
{
    this->f_newPortDataReceived = false;
    return this->payload.DATA[BYTE];        
}
