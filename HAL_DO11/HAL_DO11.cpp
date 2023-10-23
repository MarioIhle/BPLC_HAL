#include "HAL_DO11.h"

HAL_DO11::HAL_DO11()
{}

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1)
{
    this->deviceAddress     = ADDRESS;

    this->p_ports[DO11_PORT__1]   = P_DO1;    

    this->usedPorts     = 1;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_ports[DO11_PORT__1]   = P_DO1;    
    this->p_ports[DO11_PORT__2]   = P_DO2; 
    
    this->usedPorts     = 2;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3)
{
    this->deviceAddress     = ADDRESS;

    this->p_ports[DO11_PORT__1] = P_DO1;    
    this->p_ports[DO11_PORT__2] = P_DO2; 
    this->p_ports[DO11_PORT__3] = P_DO3; 
    
    this->usedPorts = 3;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4)
{
    this->deviceAddress     = ADDRESS;

    this->p_ports[DO11_PORT__1] = P_DO1;    
    this->p_ports[DO11_PORT__2] = P_DO2; 
    this->p_ports[DO11_PORT__3] = P_DO3; 
    this->p_ports[DO11_PORT__4] = P_DO4; 
    
    this->usedPorts = 4;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5)
{
    this->deviceAddress     = ADDRESS;

    this->p_ports[DO11_PORT__1] = P_DO1;    
    this->p_ports[DO11_PORT__2] = P_DO2; 
    this->p_ports[DO11_PORT__3] = P_DO3; 
    this->p_ports[DO11_PORT__4] = P_DO4; 
    this->p_ports[DO11_PORT__5] = P_DO5; 
    
    this->usedPorts = 5;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_ports[DO11_PORT__1] = P_DO1;    
    this->p_ports[DO11_PORT__2] = P_DO2; 
    this->p_ports[DO11_PORT__3] = P_DO3; 
    this->p_ports[DO11_PORT__4] = P_DO4; 
    this->p_ports[DO11_PORT__5] = P_DO5; 
    this->p_ports[DO11_PORT__6] = P_DO6; 

    this->usedPorts = 6;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6, Output* P_DO7)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_ports[DO11_PORT__1] = P_DO1;    
    this->p_ports[DO11_PORT__2] = P_DO2; 
    this->p_ports[DO11_PORT__3] = P_DO3; 
    this->p_ports[DO11_PORT__4] = P_DO4; 
    this->p_ports[DO11_PORT__5] = P_DO5; 
    this->p_ports[DO11_PORT__6] = P_DO6; 
    this->p_ports[DO11_PORT__7] = P_DO7;  

    this->usedPorts = 7;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6, Output* P_DO7, Output* P_DO8)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_ports[DO11_PORT__1] = P_DO1;    
    this->p_ports[DO11_PORT__2] = P_DO2; 
    this->p_ports[DO11_PORT__3] = P_DO3; 
    this->p_ports[DO11_PORT__4] = P_DO4; 
    this->p_ports[DO11_PORT__5] = P_DO5; 
    this->p_ports[DO11_PORT__6] = P_DO6; 
    this->p_ports[DO11_PORT__7] = P_DO7; 
    this->p_ports[DO11_PORT__8] = P_DO8; 

    this->usedPorts = 8;
}   

void HAL_DO11::begin()
{
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

    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/8");
 
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

void HAL_DO11::begin(const e_DO11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;

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

    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/8");
 
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

void HAL_DO11::mapObjectToPort(Output* P_OBJECT)
{
    this->p_ports[usedPorts] = P_OBJECT;
    this->usedPorts++;

    //Plausibilitätsprüfung
    if(this->usedPorts < DO11_PORT__COUNT)
    {
        this->errorCode = DO11_ERROR__PORT_OVERFLOW;
    }
}

void HAL_DO11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = DO11_ERROR__I2C_CONNECTION_FAILED;
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {
        for(uint8_t PORT; PORT < this->usedPorts; PORT++)
        {                
            if(this->p_ports[PORT]->isThereANewPortValue())    //Nur Wert abrufen und schreiben, falls dier sich geändert hat
            {     
                //PWM von 0-255 laden und umrechnen
                const uint16_t          TARGET_PWM_VALUE    = map(this->p_ports[PORT]->getValue().value, 0, 255, 0, 4095);

                switch(p_ports[PORT]->getOutputType())
                {
                    case OUTPUTTYPE__PULL:
                        PCA.setChannelPWM(this->PINS[PORT][LS_MOSFET], 0, TARGET_PWM_VALUE);        //lowSide
                        PCA.setChannelOff(this->PINS[PORT][HS_MOSFET]);                             //highside
                    break;

                    case OUTPUTTYPE__PUSH:
                        PCA.setChannelOff(this->PINS[PORT][LS_MOSFET]);                             //lowSide
                        PCA.setChannelPWM(this->PINS[PORT][HS_MOSFET], 0, TARGET_PWM_VALUE);        //highside
                    break;

                    case OUTPUTTYPE__PUSH_PULL:                            
                        //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                        PCA.setChannelOff(this->PINS[PORT][HS_MOSFET]); //Spannungsführend zuerst aus
                        PCA.setChannelOff(this->PINS[PORT][LS_MOSFET]);                                       
                        delay(1);       
                                                
                        //FULL OFF
                        if(TARGET_PWM_VALUE < DEAD_TIME)
                        {
                            PCA.setChannelOn(this->PINS[PORT][LS_MOSFET]);
                            //PCA.setChannelOff(this->PINS[PORT][HS_MOSFET]);    
                        }
                        //FULL ON
                        else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                        {
                            //PCA.setChannelOff(this->PINS[PORT][LS_MOSFET]);
                            PCA.setChannelOn(this->PINS[PORT][HS_MOSFET]);    
                        }
                        //PWM
                        else
                        {                        
                            PCA.setChannelPWM(this->PINS[PORT][LS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                            PCA.setChannelPWM(this->PINS[PORT][HS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                        }
                    break;
                    case OUTPUTTYPE__PUSH_PULL_INVERT:                            
                        //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                        PCA.setChannelOff(this->PINS[PORT][HS_MOSFET]); //Spannungsführend zuerst aus
                        PCA.setChannelOff(this->PINS[PORT][LS_MOSFET]);                                       
                        delay(1);       
                                                
                        //FULL OFF
                        if(TARGET_PWM_VALUE < DEAD_TIME)
                        {
                            PCA.setChannelOn(this->PINS[PORT][HS_MOSFET]);
                            //PCA.setChannelOff(this->PINS[PORT][HS_MOSFET]);    
                        }
                        //FULL ON
                        else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                        {
                            //PCA.setChannelOff(this->PINS[PORT][LS_MOSFET]);
                            PCA.setChannelOn(this->PINS[PORT][LS_MOSFET]);    
                        }
                        //PWM
                        else
                        {                        
                            PCA.setChannelPWM(this->PINS[PORT][HS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                            PCA.setChannelPWM(this->PINS[PORT][LS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                        }
                    break;
                }  
            }        
        }   
    }
}

e_BPLC_ERROR_t HAL_DO11::getError()
{
    return this->errorCode;
}