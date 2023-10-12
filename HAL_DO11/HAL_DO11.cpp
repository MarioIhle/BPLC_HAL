#include "HAL_DO11.h"

HAL_DO11::HAL_DO11()
{}

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1)
{
    this->deviceAddress     = ADDRESS;

    this->p_DO[DO_PORT_1]   = P_DO1;    

    this->usedPortCount     = 1;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_DO[DO_PORT_1]   = P_DO1;    
    this->p_DO[DO_PORT_2]   = P_DO2; 
    
    this->usedPortCount     = 2;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3)
{
    this->deviceAddress     = ADDRESS;

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    
    this->usedPortCount = 3;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4)
{
    this->deviceAddress     = ADDRESS;

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    
    this->usedPortCount = 4;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5)
{
    this->deviceAddress     = ADDRESS;

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    this->p_DO[DO_PORT_5] = P_DO5; 
    
    this->usedPortCount = 5;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    this->p_DO[DO_PORT_5] = P_DO5; 
    this->p_DO[DO_PORT_6] = P_DO6; 

    this->usedPortCount = 6;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6, Output* P_DO7)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    this->p_DO[DO_PORT_5] = P_DO5; 
    this->p_DO[DO_PORT_6] = P_DO6; 
    this->p_DO[DO_PORT_7] = P_DO7;  

    this->usedPortCount = 7;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6, Output* P_DO7, Output* P_DO8)
{
    this->deviceAddress     = ADDRESS;
    
    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    this->p_DO[DO_PORT_5] = P_DO5; 
    this->p_DO[DO_PORT_6] = P_DO6; 
    this->p_DO[DO_PORT_7] = P_DO7; 
    this->p_DO[DO_PORT_8] = P_DO8; 

    this->usedPortCount = 8;
}   

e_APP_ERROR_t HAL_DO11::begin()
{
    e_APP_ERROR_t error = APP_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.print("setup DO11 ");
    switch(this->deviceAddress)
    {
        case DO11_CARD_1:
            Serial.print("1");
        break;
        case DO11_CARD_2:
            Serial.print("2");
        break;
        case DO11_CARD_3:
            Serial.print("3");
        break;
        case DO11_CARD_4:
            Serial.print("4");
        break;
    }
    Serial.println("/4");
    Serial.print("Ports defined: "); Serial.print(this->usedPortCount); Serial.println("/8");
 
    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = APP_ERROR__DIN11_COMMUNICATION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(error == APP_ERROR__NO_ERROR)
    {        
        PCA.setI2CAddress(this->deviceAddress);
        PCA.init();
        PCA.setPWMFrequency();
        PCA.setAllChannelsPWM(0);
    }
    else
    {
        this->f_error = true;
    }

    return error;
}

void HAL_DO11::tick()
{
    this->f_error = !this->selfCheck.requestHeartbeat();

    if(!this->f_error)
    {
        for(uint8_t PORT; PORT < this->usedPortCount; PORT++)
        {                
            if(this->p_DO[PORT]->isThereANewPortValue())    //Nur Wert abrufen und schreiben, falls dier sich geändert hat
            {     
                //PWM von 0-255 laden und umrechnen
                const s_portValue_t     VALUE_TO_WRITE      = this->p_DO[PORT]->getValue();
                const uint16_t          TARGET_PWM_VALUE    = map(VALUE_TO_WRITE.value, 0, 255, 0, 4095);

                switch(p_DO[PORT]->getOutputType())
                {
                    case OUTPUTTYPE__PULL:
                        PCA.setChannelPWM(this->pins[PORT][LS_MOSFET], 0, TARGET_PWM_VALUE);        //lowSide
                        PCA.setChannelOff(this->pins[PORT][HS_MOSFET]);                             //highside
                    break;

                    case OUTPUTTYPE__PUSH:
                        PCA.setChannelOff(this->pins[PORT][LS_MOSFET]);                             //lowSide
                        PCA.setChannelPWM(this->pins[PORT][HS_MOSFET], 0, TARGET_PWM_VALUE);        //highside
                    break;

                    case OUTPUTTYPE__PUSH_PULL:                            
                        //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                        PCA.setChannelOff(this->pins[PORT][HS_MOSFET]); //Spannungsführend zuerst aus
                        PCA.setChannelOff(this->pins[PORT][LS_MOSFET]);                                       
                        delay(1);       
                                                
                        //FULL OFF
                        if(TARGET_PWM_VALUE < DEAD_TIME)
                        {
                            PCA.setChannelOn(this->pins[PORT][LS_MOSFET]);
                            //PCA.setChannelOff(this->pins[PORT][HS_MOSFET]);    
                        }
                        //FULL ON
                        else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                        {
                            //PCA.setChannelOff(this->pins[PORT][LS_MOSFET]);
                            PCA.setChannelOn(this->pins[PORT][HS_MOSFET]);    
                        }
                        //PWM
                        else
                        {                        
                            PCA.setChannelPWM(this->pins[PORT][LS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                            PCA.setChannelPWM(this->pins[PORT][HS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                        }
                    break;
                    case OUTPUTTYPE__PUSH_PULL_INVERT:                            
                        //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                        PCA.setChannelOff(this->pins[PORT][HS_MOSFET]); //Spannungsführend zuerst aus
                        PCA.setChannelOff(this->pins[PORT][LS_MOSFET]);                                       
                        delay(1);       
                                                
                        //FULL OFF
                        if(TARGET_PWM_VALUE < DEAD_TIME)
                        {
                            PCA.setChannelOn(this->pins[PORT][HS_MOSFET]);
                            //PCA.setChannelOff(this->pins[PORT][HS_MOSFET]);    
                        }
                        //FULL ON
                        else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                        {
                            //PCA.setChannelOff(this->pins[PORT][LS_MOSFET]);
                            PCA.setChannelOn(this->pins[PORT][LS_MOSFET]);    
                        }
                        //PWM
                        else
                        {                        
                            PCA.setChannelPWM(this->pins[PORT][HS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                            PCA.setChannelPWM(this->pins[PORT][LS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                        }
                    break;
                }  
            }        
        }   
    }
}

e_APP_ERROR_t HAL_DO11::getError()
{
    e_APP_ERROR_t tempError = APP_ERROR__NO_ERROR;
    if(this->f_error)
    {
        tempError = APP_ERROR__DIN11_COMMUNICATION_FAILED;
    }
    return tempError;
}