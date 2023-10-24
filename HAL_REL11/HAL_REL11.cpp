#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS, Output* P_REL1)
{
    this->p_REL[REL11_PORT__1] = P_REL1;    
    this->usedPorts = 1;

    this->deviceAddress  = I2C_ADDRESS;
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS, Output* P_REL1, Output* P_REL2)
{
    this->p_REL[REL11_PORT__1] = P_REL1;
    this->p_REL[REL11_PORT__1] = P_REL2;    
    this->usedPorts = 2;

    this->deviceAddress  = I2C_ADDRESS;
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t I2C_ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3)
{
    this->p_REL[REL11_PORT__1] = P_REL1;
    this->p_REL[REL11_PORT__2] = P_REL2;
    this->p_REL[REL11_PORT__3] = P_REL3;
    this->usedPorts = 3;

    this->deviceAddress  = I2C_ADDRESS;    
}

void HAL_REL11::begin()
{    
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

    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/3");
 
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

void HAL_REL11::begin(const e_REL11_ADDRESS_t I2C_ADDRESS)
{    
    this->deviceAddress = I2C_ADDRESS;
    
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

    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/3");
 
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

void HAL_REL11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = REL11_ERROR__I2C_CONNECTION_FAILED;
    }
    else
    {
        //this->f_error = false;    //selbrücksetellung des Fehlerzustands, zur zeit nicht genutzt
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {         
        for(int PORT = 0; PORT < this->usedPorts; PORT++)
        {
            if(this->p_REL[PORT]->isThereANewPortValue())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
            {
                if(this->p_REL[PORT]->getValue().value >= 1)
                {
                    this->PCF.write(this->PINS[PORT], true);
                }
                else if(this->p_REL[PORT]->getValue().value == false)
                {
                    this->PCF.write(this->PINS[PORT], false);
                } 

    #ifdef DEBUG_HAL_REL11 
    Serial.print("value: "); Serial.println(VALUE_TO_WRITE.value);
    Serial.print("previours value: "); Serial.println(VALUE_TO_WRITE.previousValue);
    #endif

            }               
        }   
    } 
}

e_BPLC_ERROR_t HAL_REL11::getError()
{
    return this->errorCode;
}