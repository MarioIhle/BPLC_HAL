#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_DIN11::setup()
{  
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
    if(CHANNEL < 0 || CHANNEL > DIN11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr && CHANNEL == DIN11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr)
    {
        this->setError(DIN11_ERROR__CHANNEL_ALREADY_IN_USE);       
    }
    else
    {
        if(P_IO_OBJECT->getIoType() == IO_TYPE__ROTARY_ENCODER)
        {//3 pinPorts belegen mit gleichen objekt
            this->channels.p_ioObject[CHANNEL]      = P_IO_OBJECT;
            this->channels.p_ioObject[CHANNEL + 1]  = P_IO_OBJECT;
            this->channels.p_ioObject[CHANNEL + 2]  = P_IO_OBJECT;
        }
        else
        {
            this->channels.p_ioObject[CHANNEL] = P_IO_OBJECT;
        }
    }
}
void HAL_DIN11::tick()
{   
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {           
        //PCF über I2C lesen
        this->PCF.read8();
        //Alle genutzen Channels abfragen
        for(uint8_t CH = 0; CH < DIN11_CHANNEL_COUNT; CH++)
        {      
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                u_IO_DATA_BASE_t tempBuffer;                

                switch (this->channels.p_ioObject[CH]->getIoType())
                {                    
                    case IO_TYPE__DIGITAL_INPUT:
                    case IO_TYPE__RPM_SENS:
                        tempBuffer.digitalIoData.state = !PCF.read(this->channels.PIN[CH]);
                        this->channels.p_ioObject[CH]->halCallback(tempBuffer); 
                        break;

                    case IO_TYPE__ROTARY_ENCODER:
                        tempBuffer.rotaryEncoderData.stateA             = !PCF.read(this->channels.PIN[CH]);
                        tempBuffer.rotaryEncoderData.stateB             = !PCF.read(this->channels.PIN[CH + 1]);
                        tempBuffer.rotaryEncoderData.statePushButton    = !PCF.read(this->channels.PIN[CH + 2]);
                        this->channels.p_ioObject[CH]->halCallback(tempBuffer);
                        CH +=3;
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
    return this->getError();
}