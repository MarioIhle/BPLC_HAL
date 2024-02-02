#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11()
{
    for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
    {             
        this->channels.channelState[CH] = DIN_CHANNEL_STATE__NOT_USED; 
    } 
}

void HAL_DIN11::begin(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{  
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;   
    
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
    {            
        if(this->channels.channelState[CH] == DIN_CHANNEL_STATE__NOT_USED)
        {
            break;
        }
        else if(this->channels.channelState[CH] == DIN_CHANNEL_STATE__NOT_USED && CH == (DIN11_CHANNEL__COUNT - 1))
        {
            this->errorCode = DIN11_ERROR__NO_CHANNEL_IN_USE;
        }
    }

    //I2C Verbindung prüfen
    if(I2C_check::begin(I2C_ADDRESS) == false)
    {
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
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

e_BPLC_ERROR_t HAL_DIN11::mapObjectToChannel(DigitalInput* P_OBJECT, const e_DIN11_CHANNEL_t CHANNEL)
{
    if(this->channels.channelState[CHANNEL] == DIN_CHANNEL_STATE__NOT_USED)
    {
        this->channels.p_digital[CHANNEL]       = P_OBJECT;
        this->channels.channelState[CHANNEL]    = DIN_CHANNEL_STATE__DIGITAL;
        //Vielleicht irgendwas wegen dem ISR machen? 
    }
    else 
    {
        this->errorCode = DIN11_ERROR__CHANNEL_ALREADY_IN_USE;
    }
    return this->errorCode;
}

e_BPLC_ERROR_t HAL_DIN11::mapObjectToChannel(rpmSensor* P_OBJECT, const e_DIN11_CHANNEL_t CHANNEL)
{
    if(this->channels.channelState[CHANNEL] == DIN_CHANNEL_STATE__NOT_USED)
    {
        this->channels.p_counter[CHANNEL]       = P_OBJECT;
        this->channels.channelState[CHANNEL]    = DIN_CHANNEL_STATE__COUNTER;        
    }
    else 
    {
        this->errorCode = DIN11_ERROR__CHANNEL_ALREADY_IN_USE;
    }    
    return this->errorCode;
}

void HAL_DIN11::tick()
{      
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;
    }

    if(this->inputReadRequests > 0)
    {
        //Alle genutzen Channels einlesen
        if(this->errorCode == BPLC_ERROR__NO_ERROR)
        {           
            for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
            {      
                switch (this->channels.channelState[CH])
                {
                    default:
                    case DIN_CHANNEL_STATE__NOT_USED:
                    //do nothing
                    break;

                    case DIN_CHANNEL_STATE__DIGITAL:                       
                        this->channels.p_digital[CH]->halCallback(!PCF.read(this->channels.PIN[CH])); 
                    break;

                    case DIN_CHANNEL_STATE__COUNTER:
                        this->channels.p_counter[CH]->halCallback(!PCF.read(this->channels.PIN[CH]));
                    break;               
                }                                  
            }       
        }
        this->inputReadRequests--;
    }
}

e_BPLC_ERROR_t HAL_DIN11::getError()
{
    return this->errorCode;
}

void HAL_DIN11::ISR_callback()
{
    this->inputReadRequests += READ_TWO_TIMES;
}