#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{}
void HAL_REL11::init(const e_EC_ADDR_t ADDR)
{    
    if(ADDR < REL11_ADDRESS_COUNT)
    {
        this->deviceAddress = REL11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(REL11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    for(uint8_t CH =0; CH < REL11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
    
    //I2C Verbindung Prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
        this->setError(REL11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }
    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {   
        this->PCF.setAddress(this->deviceAddress);      
        this->PCF.begin();                              
        this->PCF.write8(false);                         
        this->printLog("REL11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);       
    }
    else
    {
        this->printLog("REL11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);    
    }  
}   
void HAL_REL11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > REL11_CHANNEL_COUNT)
    {
        this->setError(REL11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == REL11_CHANNEL_COUNT)
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
    }
}
void HAL_REL11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    //Hal ticken
    if(this->noErrorSet())
    {         
        for(int CH = 0; CH < REL11_CHANNEL_COUNT; CH++)
        {
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                {
                    u_HAL_DATA_t tempBuffer;                

                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {
                        case IO_TYPE__OUTPUT_PUSH:
                            tempBuffer = this->channels.p_ioObject[CH]->halCallback();

                            if(tempBuffer.digitalIoData.state >= 1)
                            {
                                this->PCF.write(this->channels.PIN[CH], true);
                            }
                            else if(tempBuffer.digitalIoData.state == false)
                            {
                                this->PCF.write(this->channels.PIN[CH], false);
                            }
                            break;

                        default:
                        case IO_TYPE__NOT_DEFINED:
                            this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                            break;  
                    }                    
                }               
            }
        }   
    } 
}