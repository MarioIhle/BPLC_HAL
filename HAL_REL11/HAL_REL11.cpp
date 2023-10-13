#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1)
{
    this->p_REL[0] = P_REL1;    
    this->usedPortCount = 1;

    this->deviceAddress  = ADDRESS;
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;    
    this->usedPortCount = 2;

    this->deviceAddress  = ADDRESS;
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;
    this->p_REL[2] = P_REL3;
    this->usedPortCount = 3;

    this->deviceAddress  = ADDRESS;    
}

e_APP_ERROR_t HAL_REL11::begin()
{    
    e_APP_ERROR_t error = APP_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup REL11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case REL11_CARD_1:
            Serial.println("1");
        break;
        case REL11_CARD_2:
            Serial.println("2");
        break;
        case REL11_CARD_3:
            Serial.println("3");
        break;
        case REL11_CARD_4:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    Serial.print("Ports defined: "); Serial.print(this->usedPortCount); Serial.println("/3");
 
    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = APP_ERROR__REL11_COMMUNICATION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(error == APP_ERROR__NO_ERROR)
    {   
        this->PCF.setAddress(this->deviceAddress);       //Tatsächliche Adresse schreiben
        this->PCF.begin();                              //Kommunikation hetstellen
        this->PCF.write8(false);                        //Alle Ports LOW    
    
        this->f_error = false;  
    }
    else
    {
        this->f_error = true;                      
    }

    return error;
}   

void HAL_REL11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->f_error = true;
    }
    else
    {
        //this->f_error = false;    //selbrücksetellung des Fehlerzustands, zur zeit nicht genutzt
    }

    if(!this->f_error)
    {         
        for(int PORT = 0; PORT < this->usedPortCount; PORT++)
        {
            if(this->p_REL[PORT]->isThereANewPortValue())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
            {
                if(this->p_REL[PORT]->getValue().value >= 1)
                {
                    this->PCF.write(this->pins[PORT], true);
                }
                else if(this->p_REL[PORT]->getValue().value == false)
                {
                    this->PCF.write(this->pins[PORT], false);
                } 

    #ifdef DEBUG_HAL_REL11 
    Serial.print("value: "); Serial.println(VALUE_TO_WRITE.value);
    Serial.print("previours value: "); Serial.println(VALUE_TO_WRITE.previousValue);
    #endif

            }               
        }   
    } 
}

e_APP_ERROR_t HAL_REL11::getError()
{
    e_APP_ERROR_t tempError = APP_ERROR__NO_ERROR;
    if(this->f_error)
    {
        tempError = APP_ERROR__REL11_COMMUNICATION_FAILED;
    }
    return tempError;
}