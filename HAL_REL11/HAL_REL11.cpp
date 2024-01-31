#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{
    for(uint8_t CH; CH < REL11_CHANNEL__COUNT; CH++)
    {  
        this->channels.state[CH] = REL_CHANNEL_STATE__NOT_USED;
    }
}

void HAL_REL11::begin(const e_REL11_ADDRESS_t I2C_ADDRESS)
{    
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;
      

    //Übergebene Instanzen prüfen
    for(uint8_t CH = 0; CH < REL11_CHANNEL__COUNT; CH++)
    {            
        if(this->channels.state[CH] == REL_CHANNEL_STATE__DIGITAL)
        {
            //Irgenwelche Hardware Anpassung?
            break;
        }
        else if(this->channels.state[CH] == REL_CHANNEL_STATE__NOT_USED && CH == (REL11_CHANNEL__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = REL11_ERROR__NO_CHANNEL_IN_USE;
        }
    }

    //I2C Verbindung Prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
        this->errorCode = REL11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {   
        this->PCF.setAddress(this->deviceAddress);       //Tatsächliche Adresse schreiben
        this->PCF.begin();                              //Kommunikation hetstellen
        this->PCF.write8(false);                        //Alle Ports LOW    
        BPLC_LOG logPrint;
        logPrint.printLog("REL11revA CARD (" + String(I2C_ADDRESS) + ") INIT SUCCESSFUL");       
    }
    else
    {
        BPLC_LOG logPrint;
        logPrint.printLog("REL11revA CARD (" + String(I2C_ADDRESS) + ") INIT FAILED");    
    }  
}   

e_BPLC_ERROR_t HAL_REL11::mapObjectToChannel(Output* P_OBJECT, const uint8_t CHANNEL)
{
    if(this->channels.state[CHANNEL] == REL_CHANNEL_STATE__NOT_USED)
    {
        this->channels.p_object[CHANNEL]    = P_OBJECT;
        this->channels.state[CHANNEL]       = REL_CHANNEL_STATE__DIGITAL;
    }
    else 
    {
        this->errorCode = REL11_ERROR__CHANNEL_ALREADY_IN_USE;
    }
    return this->errorCode;
}

void HAL_REL11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!I2C_check::requestHeartbeat())
    {
        this->errorCode = REL11_ERROR__I2C_CONNECTION_FAILED;
    }    

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {         
        for(int CH = 0; CH < REL11_CHANNEL__COUNT; CH++)
        {
            if(this->channels.state[CH] == REL_CHANNEL_STATE__DIGITAL)
            {
                if(this->channels.p_object[CH]->isThereANewPortValue())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                {
                    if(this->channels.p_object[CH]->halCallback().value >= 1)
                    {
                        this->PCF.write(this->channels.PIN[CH], true);
                    }
                    else if(this->channels.p_object[CH]->halCallback().value == false)
                    {
                        this->PCF.write(this->channels.PIN[CH], false);
                    }
                }               
            }
        }   
    } 
}

e_BPLC_ERROR_t HAL_REL11::getError()
{
    return this->errorCode;
}