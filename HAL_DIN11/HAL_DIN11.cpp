#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_DIN11::setup()
{  
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;   

    //I2C Verbindung prüfen
    if(I2C_check::begin(I2C_ADDRESS) == false)
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED);        
    }

    //Applikationsparameter initialisieren
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();      
        this->printLog("DIN11revA CARD (" + String(I2C_ADDRESS) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        this->printLog("DIN11revA CARD (" + String(I2C_ADDRESS) + ") INIT FAILED");
    }
}
void HAL_DIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    //Wenn Channel 1 übergeben wird, ist p_ioObject[0] gemeint 
    CHANNEL--;
    if(CHANNEL < 0 || CHANNEL > DIN11_CHANNEL__COUNT)
    {
        this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr && CHANNEL == DIN11_CHANNEL__COUNT)
    {
        this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr)
    {
        this->setError(DIN11_ERROR__CHANNEL_ALREADY_IN_USE);       
    }
    else
    {
        this->channels.p_ioObject[CHANNEL] = P_IO_OBJECT;
    }
}
void HAL_DIN11::mapObjectToChannel(rpmSensor* P_OBJECT, const e_DIN11_CHANNEL_t CHANNEL)
{
    if(this->channels.channelState[CHANNEL] == DIN_CHANNEL_STATE__NOT_USED)
    {
        this->channels.p_digital[CHANNEL]       = P_OBJECT;
        this->channels.channelState[CHANNEL]    = DIN_CHANNEL_STATE__DIGITAL;
    }
    else 
    {
        this->errorCode = DIN11_ERROR__CHANNEL_ALREADY_IN_USE;
    }
}
void HAL_DIN11::tick()
{   
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {           
        //PCF über I2C lesen
        this->PCF.read8();
        //Alle genutzen Channels abfragen
        for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
        {      
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                switch (this->channels.p_ioObject[CH]->getIoType())
                {                    
                    case IO_TYPE__DIGITAL_INPUT:
                    case IO_TYPE__RPM_SENS:
                        this->channels.p_ioObject[CH]->halCallback(!PCF.read(this->channels.PIN[CH])); 
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
e_BPLC_ERROR_t HAL_DIN11::getError()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->errorCode;
}