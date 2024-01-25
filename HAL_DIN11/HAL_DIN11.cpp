#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11()
{
    for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
    {             
        this->channels.channelState[CH] = CHANNEL_OBJECT_TYPE__NOT_USED; 
    } 
}

void HAL_DIN11::begin(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{  
    this->deviceAddress                 = I2C_ADDRESS;
    this->errorCode                     = BPLC_ERROR__NO_ERROR;

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

e_BPLC_ERROR_t HAL_DIN11::mapObjectToChannel(DigitalInput* P_OBJECT, const e_DIN11_CHANNEL_t CHANNEL)
{
    if(this->channels.channelState[CHANNEL] == CHANNEL_OBJECT_TYPE__NOT_USED)
    {
        this->channels.p_digital[CHANNEL]       = P_OBJECT;
        this->channels.channelState[CHANNEL]    = CHANNEL_OBJECT_TYPE__DIGITAL;
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
    if(this->channels.channelState[CHANNEL] == CHANNEL_OBJECT_TYPE__NOT_USED)
    {
        this->channels.p_counter[CHANNEL]       = P_OBJECT;
        this->channels.channelState[CHANNEL]    = CHANNEL_OBJECT_TYPE__COUNTER;        
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
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;
    }

    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
    {            
        if(this->channels.channelState[CH] == CHANNEL_OBJECT_TYPE__NOT_USED)
        {
            break;
        }
        else if(this->channels.channelState[CH] == CHANNEL_OBJECT_TYPE__NOT_USED && CH == (DIN11_CHANNEL__COUNT - 1))
        {
            this->errorCode = DIN11_ERROR__NO_CHANNEL_IN_USE;
        }
    }

    //Alle genutzen Channels einlesen
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {           
        for(uint8_t CH = 0; CH < DIN11_CHANNEL__COUNT; CH++)
        {      
            switch (this->channels.channelState[CH])
            {
                default:
                case CHANNEL_OBJECT_TYPE__NOT_USED:
                //do nothing
                break;

                case CHANNEL_OBJECT_TYPE__DIGITAL:                       
                    this->channels.p_digital[CH]->halCallback(!PCF.read(this->channels.PIN[CH])); 
                break;

                case CHANNEL_OBJECT_TYPE__COUNTER:
                    this->channels.p_counter[CH]->halCallback(!PCF.read(this->channels.PIN[CH]));
                break;               
            }                                  
        }       
    }
}

e_BPLC_ERROR_t HAL_DIN11::getError()
{
    return this->errorCode;
}