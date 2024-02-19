#include "HAL_TMP11.h"

HAL_TMP11::HAL_TMP11(const e_TMP11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_TMP11::init()
{    
    for(uint8_t CH =0; CH < TMP11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
    
    //I2C Verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
       this->setError(TMP11_ERROR__I2C_CONNECTION_FAILED);        
    }
    //Applikationsparameter initialisieren         
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {   
        this->printLog("TMP11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        this->printLog("TMP11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED");    
    }
}
void HAL_TMP11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > TMP11_CHANNEL_COUNT)
    {
        this->setError(TMP11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        if(CHANNEL == TMP11_CHANNEL_COUNT)
        {
            this->setError(TMP11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
        }
        else
        {
            this->setError(TMP11_ERROR__CHANNEL_ALREADY_IN_USE);
        }       
    }
    else
    { 
        switch (P_IO_OBJECT->getIoType())
        {         
            case IO_TYPE__PTC:
            case IO_TYPE__PT100:       
            case IO_TYPE__PT1000:    
                this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;        
            break;
            
            default:
                this->setError(TMP11_ERROR__IO_OBJECT_NOT_SUITABLE);
            break;               
        }        
    }
}
void HAL_TMP11::tick()
{   
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {          
        for(uint8_t CH = 0; CH < TMP11_CHANNEL_COUNT; CH++)
        {            
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())
                {
                    u_HAL_CALLBACK_DATA_t tempBuffer;

                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {         
                        case IO_TYPE__PTC:
                        break;

                        case IO_TYPE__PT100:        
                        break;
                           
                        case IO_TYPE__PT1000:    
                            tempBuffer.analogIoData.value = this->ADC.readADC_SingleEnded(this->channels.PIN[CH]);
                            if(tempBuffer.analogIoData.value >= 0)
                            {
                                this->channels.p_ioObject[CH]->halCallback(&tempBuffer);                        
                            }     
                            else
                            {
                                tempBuffer.analogIoData.value = 0;
                                this->channels.p_ioObject[CH]->halCallback(&tempBuffer);
                            }         
                        break;
                        
                        default:
                        case IO_TYPE__NOT_DEFINED:
                            this->setError(TMP11_ERROR__IO_OBJECT_NOT_SUITABLE);
                        break;               
                    }
                }       
            }
        }  
    }
}
e_BPLC_ERROR_t HAL_TMP11::getErrorCode()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(TMP11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}