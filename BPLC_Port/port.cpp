#include "BPLC_Port.h"

//---------------------------------------------------------------------------------------
//BERTANET EVENTPORT KLASSE
//---------------------------------------------------------------------------------------
//STANDART CONSTRUCTOR
BPLC_COM_Port::BPLC_COM_Port	()
{}      
//EVENTPORT CONSTRUCTOR
BPLC_COM_Port::BPLC_COM_Port(e_PortType_t TYPE, uint8_t PORT_INDEX)
{
    this->type                      = TYPE;
    this->index                     = PORT_INDEX;
    this->f_newPortDataReceived     = false;
    this->f_newPortDataToSend       = false;
    this->f_portDataRequested       = false;
    memset(&this->payload, 0, sizeof(u_Payload_t));
}

//PUSHPORT CONSTRUCTOR
BPLC_COM_Port::BPLC_COM_Port(e_PortType_t TYPE, uint8_t PORT_INDEX, uint32_t PUSH_INTERVALL)
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
uint8_t	BPLC_COM_Port::getIndex()
{
    return this->index;
}

e_PortType_t BPLC_COM_Port::getType()
{
    return this->type;
}


void BPLC_COM_Port::pushNow()
{
    this->to_push.now();
}

bool  BPLC_COM_Port::newPortDataAvailable()
{
    return this->f_newPortDataReceived;
}

bool BPLC_COM_Port::isNewPortDataAvailableToSend()
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
void BPLC_COM_Port::portDataStoredInRequestBuffer()
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
void BPLC_COM_Port::read()
{
    this->f_portDataRequested = true;
}

bool BPLC_COM_Port::isPortDataReqeuested()
{
    return this->f_portDataRequested;
}   

void BPLC_COM_Port::requestStoredInRequestBuffer()
{
    this->f_portDataRequested = false;
}



uint8_t* BPLC_COM_Port::getDataLocation()
{
    return &this->payload.DATA[0];
}


//---------------------------------------------------------------------------------------
//SETTER
//Wird von Network genutzt um empfangene Daten zu speichern
void BPLC_COM_Port::setPayload(u_Payload_t PAYLOAD)
{
    this->payload = PAYLOAD;
    this->f_newPortDataReceived = true;   
}
//______________________________________________________________________________________
void BPLC_COM_Port::setBool(bool PAYLOAD)
{
    if(PAYLOAD != this->payload.BOOL)
    {
        this->payload.BOOL = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }    
}    
//______________________________________________________________________________________
void BPLC_COM_Port::setByte(uint8_t PAYLOAD)
{    
    if(PAYLOAD != this->payload.BYTE)
    {
        this->payload.BYTE = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }
}
//______________________________________________________________________________________
void BPLC_COM_Port::setInt(int PAYLOAD)
{    
    if(PAYLOAD != this->payload.INT)
    {
        this->payload.INT = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }    
}
//______________________________________________________________________________________
void BPLC_COM_Port::setFloat(float PAYLOAD)
{    
    if(PAYLOAD != this->payload.FLOAT)
    {
        this->payload.FLOAT = PAYLOAD;
        this->f_newPortDataToSend = true;    
    }
}
//______________________________________________________________________________________
void BPLC_COM_Port::setRgb(s_RGB_t PAYLOAD)
{    
    if(PAYLOAD.red != this->payload.RGB.red || PAYLOAD.green != this->payload.RGB.green || PAYLOAD.blue != this->payload.RGB.blue)
    {
        this->payload.RGB = PAYLOAD;
        this->f_newPortDataToSend = true;    
    } 
}
//______________________________________________________________________________________
void BPLC_COM_Port::setData(uint8_t BYTE, uint8_t PAYLOAD)
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
u_Payload_t BPLC_COM_Port::getPayload()
{  
    return this->payload;         
}

bool BPLC_COM_Port::getBool()
{
    this->f_newPortDataReceived = false;
    return this->payload.BOOL;        
}

uint8_t BPLC_COM_Port::getByte()
{
    this->f_newPortDataReceived = false;
    return this->payload.BYTE;        
}

int BPLC_COM_Port::getInt()
{
    this->f_newPortDataReceived = false;
    return this->payload.INT;        
}

float BPLC_COM_Port::getFloat()
{
    this->f_newPortDataReceived = false;
    return this->payload.FLOAT;        
}

s_RGB_t BPLC_COM_Port::getRgb()
{
    this->f_newPortDataReceived = false;
    return this->payload.RGB;        
}

uint8_t BPLC_COM_Port::getData(uint8_t BYTE)
{
    this->f_newPortDataReceived = false;
    return this->payload.DATA[BYTE];        
}
