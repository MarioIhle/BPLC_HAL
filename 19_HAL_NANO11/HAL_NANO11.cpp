#include "HAL_NANO11.h"

HAL_NANO11::HAL_NANO11()
{}
void HAL_NANO11::init(const e_EC_ADDR_t ADDR)
{    
    if(ADDR < NANO11_ADDRESS_COUNT)
    {
        this->deviceAddress = NANO11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(NANO11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    for(uint8_t CH =0; CH < NANO11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }   
        
    //I2C Verbindung prüfen
    /*
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        //this->setError(NANO11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }
    */

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {   
        this->bplcNode.begin();
        this->printLog("NANO11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);        
    }    
    else
    {
        this->printLog("NANO11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);  
    } 
}
void HAL_NANO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > NANO11_CHANNEL_COUNT)
    {
        this->setError(REL11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == NANO11_CHANNEL_COUNT)
    {
        this->setError(REL11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(REL11_ERROR__CHANNEL_ALREADY_IN_USE, __FILENAME__, __LINE__);       
    }
    else
    {
        this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;   

        //Neue anzahl an Channels komunizieren, damit bei Abfrage auch genügned Daten geschickt werden
      /*
        s_NANO11_COMMAND_t command;
        command.extract.key                         = NANO11_COMMAND_KEY__SET_CHANNEL_COUNT;
        command.extract.payload.analogIoData.value  = (uint32_t)CHANNEL;
        command.extract.channel                     = 0;
        const uint8_t BYTE_COUNT                    = sizeof(command);
        this->bplcNode.sendCommand(this->deviceAddress, &command.data[0], BYTE_COUNT);    */ 
    }
}
void HAL_NANO11::tick()
{          
    //I2C Verbindung zyklisch prüfen
    /*
    if(!this->tickHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);      
    }
    */
    
    //Hal ticken
    if(this->noErrorSet())
    {    
        //Prüfen ob IO Objekt Daten anfordert  
        bool    requestData     = false;

        for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
        {           
            if(this->channels.p_ioObject[CH] != nullptr)
            {                
                if(this->channels.p_ioObject[CH]->newDataAvailable())
                {
                    requestData = true;       
                    break;         
                }
            }
        }

        if(requestData)
        {
            //Daten anfragen
            u_HAL_DATA_t dataBuffer[NANO11_CHANNEL_COUNT];
            this->bplcNode.getSlaveData(this->deviceAddress, &dataBuffer[0].data[0], sizeof(dataBuffer));
            
            //Daten auf Input IO Objekte übergeben, output Objekte dürfen nicht gecalled werden sonst werden States nicht geschrieben
            for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
            {
                if(this->channels.p_ioObject[CH] != nullptr)
                {  
                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {
                        case IO_TYPE__ANALOG_INPUT:
                        case IO_TYPE__DIGITAL_COUNTER:
                        case IO_TYPE__DIGITAL_INPUT:
                        case IO_TYPE__POSITION_ENCODER:
                        case IO_TYPE__PT1000:
                        case IO_TYPE__PT100:
                        case IO_TYPE__PTC:
                        case IO_TYPE__ROTARY_ENCODER:
                        case IO_TYPE__RPM_SENS:       
                            this->channels.p_ioObject[CH]->halCallback(&dataBuffer[CH]);
                            break;
                        
                        default:
                            break;
                    }          
                }  
            } 
        }       

        //Outputs schreiben
        for(int CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
        {
            if(this->channels.p_ioObject[CH] != nullptr)
            {    
                switch (this->channels.p_ioObject[CH]->getIoType())
                {
                    case IO_TYPE__OUTPUT_PUSH:                    
                    case IO_TYPE__OUTPUT_PULL:
                    case IO_TYPE__OUTPUT_PUSH_PULL:
                    case IO_TYPE__OUTPUT_PUSH_PULL_INVERT:
                    case IO_TYPE__SERVO:
                    case IO_TYPE__DC_DRIVE:
                        if(this->channels.p_ioObject[CH]->newDataAvailable())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                        {                       
                            s_NANO11_COMMAND_t command;
                            command.extract.key         = NANO11_COMMAND_KEY__WRITE;
                            command.extract.channel     = CH;
                            command.extract.payload     = this->channels.p_ioObject[CH]->halCallback();
                            const uint8_t BYTE_COUNT    = sizeof(command);
                            this->bplcNode.sendCommand(this->deviceAddress, &command.data[0], BYTE_COUNT);
                        }                       
                        break;

                    default:                    
                        break;  
                }                           
            }
        }   
    }        
}
void HAL_NANO11::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {       
        default:
            this->printLog("WRONG COMMAND FOR THIS EXTENSION CARD", __FILENAME__, __LINE__);
            break;
    }
}
