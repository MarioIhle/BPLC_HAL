#include "HAL_NANO11.h"

HAL_NANO11::HAL_NANO11()
{}
bool HAL_NANO11::init(const e_EC_ADDR_t ADDR)
{    
    this->bplcAddress = ADDR;
    
    if(ADDR < NANO11_ADDRESS_COUNT)
    {
        this->i2cAddress = NANO11_I2C_ADDRESSES[ADDR];             
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
    if(!I2C_check::begin(this->i2cAddress))
    {
        this->setError(NANO11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {   
        this->bplcNode.begin();        
        this->printLog("NANO11revA CARD (" + String(this->bplcAddress + 1 )  + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);        
    }    
    else
    {
        this->printLog("NANO11revA CARD (" + String(this->bplcAddress + 1 )  + ") INIT FAILED", __FILENAME__, __LINE__);  
    } 
    return this->noErrorSet();
}
bool HAL_NANO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    bool error = true;

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
        error = false;
    }
    return error;
}
void HAL_NANO11::tick(const bool READ_INPUTS)
{          
    //I2C Verbindung zyklisch prüfen    
    if(!this->tickHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);      
    }    
    
    //Hal ticken
    if(this->noErrorSet())
    {            
        //Daten auf Input IO Objekte übergeben, output Objekte dürfen nicht gecalled werden sonst werden States nicht geschrieben
        for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
        {           
            u_HAL_DATA_t dataBuffer;
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
                    case IO_TYPE__HMI_ENCODER:
                    case IO_TYPE__RPM_SENS:   
                        if(READ_INPUTS || this->channels.p_ioObject[CH]->newDataAvailable())
                        {                    
                            this->bplcNode.getSlaveData(this->i2cAddress, CH, &dataBuffer.data[0], sizeof(dataBuffer));
                            Serial.println("Channel " +String(CH) + " Data: " + String(dataBuffer.digitalIoData.state));    
                            this->channels.p_ioObject[CH]->setHalData(&dataBuffer);
                        }
                        break;      
                       
                    case IO_TYPE__OUTPUT_PUSH:                    
                    case IO_TYPE__OUTPUT_PULL:
                    case IO_TYPE__OUTPUT_PUSH_PULL:
                    case IO_TYPE__OUTPUT_PUSH_PULL_INVERT:
                    case IO_TYPE__SERVO:
                    case IO_TYPE__DC_DRIVE:
                        if(this->channels.p_ioObject[CH]->newDataAvailable())   //Nur Wert abrufen und schreiben, falls dieser sich geändert hat
                        {                      
                            s_NANO11_COMMAND_t command;
                            memset(&command, 0, sizeof(s_NANO11_COMMAND_t));
                           
                            command.extract.key         = (uint8_t)NANO11_COMMAND_KEY__WRITE;
                            command.extract.channel     = CH;     

                            const u_HAL_DATA_t DATA = this->channels.p_ioObject[CH]->getHalData();                           
                            memcpy(command.extract.payload, DATA.data, 12);
                           
                           /*
                            for(int i = 0; i< 14; i++)
                            {
                                Serial.print(command.data[i]);
                            }*/

                            const uint8_t BYTE_COUNT = sizeof(s_NANO11_COMMAND_t);
                            this->bplcNode.sendCommand(this->i2cAddress, &command.data[0], BYTE_COUNT);
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
