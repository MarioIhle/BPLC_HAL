#include "HAL_TMP11.h"

HAL_TMP11::HAL_TMP11()
{}
void HAL_TMP11::init(const e_EC_ADDR_t ADDR)
{    
    switch(ADDR)
    {
        case EC_ADDR_1:
            this->deviceAddress = I2C_ADDRESS_TMP11__ADDR_1;
            break;
        case EC_ADDR_2:
            this->deviceAddress = I2C_ADDRESS_TMP11__ADDR_2;
            break;
        case EC_ADDR_3:
            this->deviceAddress = I2C_ADDRESS_TMP11__ADDR_3;
            break;
        case EC_ADDR_4:
            this->deviceAddress = I2C_ADDRESS_TMP11__ADDR_4;
            break;
            
        default:
            this->setError(TMP11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
            break;
    }

    for(uint8_t CH =0; CH < TMP11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
    
    //I2C Verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
       this->setError(TMP11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }
    //Applikationsparameter initialisieren         
    if(this->noErrorSet())
    {   
        this->printLog("TMP11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);      
    }    
    else
    {
        this->printLog("TMP11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);    
    }
}
void HAL_TMP11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > TMP11_CHANNEL_COUNT)
    {
        this->setError(TMP11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        if(CHANNEL == TMP11_CHANNEL_COUNT)
        {
            this->setError(TMP11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
        }
        else
        {
            this->setError(TMP11_ERROR__CHANNEL_ALREADY_IN_USE, __FILENAME__, __LINE__);
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
                this->setError(TMP11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
            break;               
        }        
    }
}
void HAL_TMP11::tick()
{   
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    //Hal ticken
    if(this->noErrorSet())
    {          
        for(uint8_t CH = 0; CH < TMP11_CHANNEL_COUNT; CH++)
        {            
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())
                {
                    u_HAL_DATA_t tempBuffer;

                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {         
                        case IO_TYPE__PTC:
                        break;

                        case IO_TYPE__PT100:        
                        break;
                           
                        case IO_TYPE__PT1000:    
                            //tempBuffer.analogIoData.value = readMCP();
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
                            this->setError(TMP11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                        break;               
                    }
                }       
            }
        }  
    }
}