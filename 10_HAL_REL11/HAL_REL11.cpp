#include "HAL_REL11.h"

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_REL11::init()
{    
    for(uint8_t CH =0; CH < REL11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
    
    //I2C Verbindung Prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
        this->setError(REL11_ERROR__I2C_CONNECTION_FAILED);        
    }
    //Applikationsparameter initialisieren
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {   
        this->PCF.setAddress(this->deviceAddress);      
        this->PCF.begin();                              
        this->PCF.write8(false);                         
        this->printLog("REL11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL");       
    }
    else
    {
        this->printLog("REL11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED");    
    }  
}   
void HAL_REL11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > REL11_CHANNEL_COUNT)
    {
        this->setError(REL11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == REL11_CHANNEL_COUNT)
    {
        this->setError(REL11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(REL11_ERROR__CHANNEL_ALREADY_IN_USE);       
    }
    else
    {
        this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;        
    }
}
void HAL_REL11::tick()
{
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {         
        for(int CH = 0; CH < REL11_CHANNEL_COUNT; CH++)
        {
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                {
                    u_IO_DATA_BASE_t tempBuffer;                

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
                            this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE);
                            break;  
                    }                    
                }               
            }
        }   
    } 
}
e_BPLC_ERROR_t HAL_REL11::getErrorCode()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}