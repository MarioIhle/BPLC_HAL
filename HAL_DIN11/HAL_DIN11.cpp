#include "HAL_DIN11.h"

#define READ_TWO_TIMES 2

HAL_DIN11::HAL_DIN11()
{
    memset(&this->ports, 0, sizeof(this->ports));
}

void HAL_DIN11::begin(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{  
    this->deviceAddress                 = I2C_ADDRESS;
    this->errorCode                     = BPLC_ERROR__NO_ERROR;
    this->f_somePinOfsomeDinCardChanged = READ_TWO_TIMES;

    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup DIN11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case DIN11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case DIN11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case DIN11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case DIN11_CARD_4_ADDRESS:
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
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();            
    }
}

void HAL_DIN11::mapObjectToNextFreePort(DigitalInput* P_OBJECT)
{  
    #ifdef DEBUG_HAL_DIN11
    Serial.println("##############################");  
    Serial.print("DIN11 CARD: ");
    switch(this->deviceAddress)
    {
        case DIN11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case DIN11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case DIN11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case DIN11_CARD_4_ADDRESS:
            Serial.println("4");
        break;
    }
    #endif
    for(uint8_t PORT = 0; PORT < DIN11_PORT__COUNT; PORT++)
    {
        #ifdef DEBUG_HAL_DIN11
        Serial.print("PORT: "); Serial.print(PORT);
        #endif
        if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
        {
            #ifdef DEBUG_HAL_DIN11
            Serial.println(" not defined yet");
            #endif
            this->ports.p_object[PORT] = P_OBJECT;
            this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT && PORT == DIN11_PORT__8)
        {
            this->errorCode = DIN11_ERROR__PORT_OVERFLOW;
        }
        else
        {
            #ifdef DEBUG_HAL_DIN11
            Serial.println(" already defined");
            #endif
        }
    }
}

void HAL_DIN11::mapObjectToSpecificPort(DigitalInput* P_OBJECT, const uint8_t PORT)
{
    if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
    {
        this->ports.p_object[PORT] = P_OBJECT;
        this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
    }
    else 
    {
        this->errorCode = DIN11_ERROR__PORT_ALREADY_DEFINED;
    }
}

void HAL_DIN11::tick()
{      
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;
    }
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t PORT = 0; PORT < DIN11_PORT__COUNT; PORT++)
    {            
        if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
        {
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE && PORT == (DIN11_PORT__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = DIN11_ERROR__NO_PORT_IN_USE;
        }
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {           
        if(this->f_somePinOfsomeDinCardChanged > 0)
        {
            for(uint8_t PORT = 0; PORT < DIN11_PORT__COUNT; PORT++)
            {      
                if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)   
                {
                    //Serial.print("DI11 PORT: "); Serial.print(PORT); Serial.print(" State: "); Serial.println(!PCF.read(this->ports.PIN[PORT]));
                    const bool STATE = !PCF.read(this->PIN[PORT]);     
                    this->ports.p_object[PORT]->setPortState(STATE);   
                }                   
            } 
            this->f_somePinOfsomeDinCardChanged--;
        }    
    }
}

void HAL_DIN11::somePinOfsomeDinCardChanged()
{
    this->f_somePinOfsomeDinCardChanged = READ_TWO_TIMES;
}

e_BPLC_ERROR_t HAL_DIN11::getError()
{
    return this->errorCode;
}