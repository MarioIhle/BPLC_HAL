#include "HAL_DO11.h"

HAL_DO11::HAL_DO11()
{
    memset(&this->ports, 0, sizeof(this->ports));
}

void HAL_DO11::begin(const e_DO11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;

    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup DO11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case DO11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case DO11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case DO11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case DO11_CARD_4_ADDRESS:
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
        this->errorCode = DO11_ERROR__I2C_CONNECTION_FAILED;     
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {        
        PCA.setI2CAddress(this->deviceAddress);
        PCA.init();
        PCA.setPWMFrequency();
        PCA.setAllChannelsPWM(0);
    }
}

e_BPLC_ERROR_t HAL_DO11::mapObjectToNextFreePort(Output* P_OBJECT)
{  
    for(uint8_t PORT = 0; PORT < DO11_PORT__COUNT; PORT++)
    {
        if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
        {
            this->ports.p_object[PORT] = P_OBJECT;
            this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT && PORT == DO11_PORT__8)
        {
            this->errorCode = DO11_ERROR__PORT_OVERFLOW;
        }
    }
    return this->errorCode;
}

void HAL_DO11::setPWMFreqency(const uint8_t FREQUENCY)
{
    this->PCA.setPWMFreqency(FREQUENCY);
}

e_BPLC_ERROR_t HAL_DO11::mapObjectToSpecificPort(Output* P_OBJECT, const e_DO11_PORTS_t PORT)
{
    if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
    {
        this->ports.p_object[PORT] = P_OBJECT;
        this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
    }
    else 
    {
        this->errorCode = DO11_ERROR__PORT_ALREADY_DEFINED;
    }
    return this->errorCode;
}

void HAL_DO11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = DO11_ERROR__I2C_CONNECTION_FAILED;
    }
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t PORT = 0; PORT < DO11_PORT__COUNT; PORT++)
    {            
        if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
        {
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE && PORT == (DO11_PORT__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = DO11_ERROR__NO_PORT_IN_USE;
        }
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {
        for(uint8_t PORT; PORT < DO11_PORT__COUNT; PORT++)
        {       
            if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
            {                      
                if(this->ports.p_object[PORT]->isThereANewPortValue())    //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                {     
                    //PWM von 0-255 laden und umrechnen
                    const uint16_t TARGET_PWM_VALUE = map(this->ports.p_object[PORT]->getValue().value, 0, 255, 0, 4095);

                    switch(this->ports.p_object[PORT]->getOutputType())
                    {
                        case OUTPUTTYPE__PULL:
                            PCA.setChannelPWM(this->PIN[PORT][LS_MOSFET], 0, TARGET_PWM_VALUE);        //lowSide
                            PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]);                             //highside
                        break;

                        case OUTPUTTYPE__PUSH:
                            PCA.setChannelOff(this->PIN[PORT][LS_MOSFET]);                             //lowSide
                            PCA.setChannelPWM(this->PIN[PORT][HS_MOSFET], 0, TARGET_PWM_VALUE);        //highside
                        break;

                        case OUTPUTTYPE__PUSH_PULL:                            
                            //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                            PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]); //Spannungsführend zuerst aus
                            PCA.setChannelOff(this->PIN[PORT][LS_MOSFET]);                                       
                            delay(1);       
                                                    
                            //FULL ON
                            if(TARGET_PWM_VALUE < DEAD_TIME)
                            {
                                PCA.setChannelOn(this->PIN[PORT][LS_MOSFET]);
                                //PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]);    
                            }
                            //FULL OFF
                            else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                            {
                                //PCA.setChannelOff(this->PIN[PORT][LS_MOSFET]);
                                PCA.setChannelOn(this->PIN[PORT][HS_MOSFET]);    
                            }
                            //PWM
                            else
                            {                        
                                PCA.setChannelPWM(this->PIN[PORT][LS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                                PCA.setChannelPWM(this->PIN[PORT][HS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                            }
                        break;
                        case OUTPUTTYPE__PUSH_PULL_INVERT:                            
                            //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                            PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]); //Spannungsführend zuerst aus
                            PCA.setChannelOff(this->PIN[PORT][LS_MOSFET]);                                       
                            delay(1);       
                                                    
                            //FULL ON
                            if(TARGET_PWM_VALUE < DEAD_TIME)
                            {
                                PCA.setChannelOn(this->PIN[PORT][HS_MOSFET]);
                                //PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]);    
                            }
                            //FULL OFF
                            else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                            {
                                //PCA.setChannelOff(this->PIN[PORT][LS_MOSFET]);
                                PCA.setChannelOn(this->PIN[PORT][LS_MOSFET]);    
                            }
                            //PWM
                            else
                            {                        
                                PCA.setChannelPWM(this->PIN[PORT][HS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                                PCA.setChannelPWM(this->PIN[PORT][LS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                            }
                        break;
                    }
                }  
            }        
        }   
    }
}

e_BPLC_ERROR_t HAL_DO11::getError()
{
    return this->errorCode;
}