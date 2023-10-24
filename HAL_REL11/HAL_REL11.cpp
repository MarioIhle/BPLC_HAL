#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{
    memset(&this->ports, 0, sizeof(this->ports));
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

void HAL_REL11::mapObjectToNextFreePort(Output* P_OBJECT)
{  
    for(uint8_t PORT = 0; PORT < REL11_PORT__COUNT; PORT++)
    {
        if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
        {
            this->ports.p_object[PORT] = P_OBJECT;
            this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT && PORT == REL11_PORT__3)
        {
            this->errorCode = REL11_ERROR__PORT_OVERFLOW;
        }
    }
}

void HAL_REL11::mapObjectToSpecificPort(Output* P_OBJECT, const uint8_t PORT)
{
    if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
    {
        this->ports.p_object[PORT] = P_OBJECT;
        this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
    }
    else 
    {
        this->errorCode = REL11_ERROR__PORT_ALREADY_DEFINED;
    }
}

void HAL_REL11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = REL11_ERROR__I2C_CONNECTION_FAILED;
    }
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t PORT = 0; PORT < REL11_PORT__COUNT; PORT++)
    {            
        if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
        {
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE && PORT == (REL11_PORT__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = REL11_ERROR__NO_PORT_IN_USE;
        }
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {         
        for(int PORT = 0; PORT < REL11_PORT__COUNT; PORT++)
        {
            if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
            {
                if(this->ports.p_object[PORT]->isThereANewPortValue())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                {
                    if(this->ports.p_object[PORT]->getValue().value >= 1)
                    {
                        this->PCF.write(this->PIN[PORT], true);
                    }
                    else if(this->ports.p_object[PORT]->getValue().value == false)
                    {
                        this->PCF.write(this->PIN[PORT], false);
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