#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{
    for(uint8_t CH; CH < REL11_CHANNEL__COUNT; CH++)
    {  
        this->channels.used[CH] = CHANNEL_STATE__NOT_IN_USE;
    }
}

void HAL_REL11::begin(const e_REL11_ADDRESS_t I2C_ADDRESS)
{    
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup REL11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case REL11_CARD__1:
            Serial.println("1");
        break;
        case REL11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case REL11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case REL11_CARD_4_ADDRESS:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        this->errorCode = REL11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {   
        this->PCF.setAddress(this->deviceAddress);       //Tatsächliche Adresse schreiben
        this->PCF.begin();                              //Kommunikation hetstellen
        this->PCF.write8(false);                        //Alle Ports LOW    
    }
}   

e_BPLC_ERROR_t HAL_REL11::mapObjectToChannel(Output* P_OBJECT, const uint8_t CHANNEL)
{
    if(this->channels.used[CHANNEL] == CHANNEL_STATE__NOT_IN_USE)
    {
        this->channels.p_object[CHANNEL] = P_OBJECT;
        this->channels.used[CHANNEL]     = CHANNEL_STATE__MAPPED_TO_OBJECT;
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
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = REL11_ERROR__I2C_CONNECTION_FAILED;
    }
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t CH = 0; CH < REL11_CHANNEL__COUNT; CH++)
    {            
        if(this->channels.used[CH] == CHANNEL_STATE__MAPPED_TO_OBJECT)
        {
            break;
        }
        else if(this->channels.used[CH] == CHANNEL_STATE__NOT_IN_USE && CH == (REL11_CHANNEL__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = REL11_ERROR__NO_CHANNEL_IN_USE;
        }
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {         
        for(int CH = 0; CH < REL11_CHANNEL__COUNT; CH++)
        {
            if(this->channels.used[CH] == CHANNEL_STATE__MAPPED_TO_OBJECT)
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