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

void HAL_DO11::begin()
{
    PCA.setI2CAddress(this->deviceAddress);
    PCA.init();
    PCA.setPWMFrequency();
    PCA.setAllChannelsPWM(0);
}

void HAL_DO11::tick()
{
    for(uint8_t PORT; PORT < this->usedPortCount; PORT++)
    {        
        this->p_DO[PORT]->tick();   //Alle OUTs müssen getickt werden, damit ein Blinken möglich wird        
                    
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
                    PCA.setChannelOff(this->pins[PORT][LS_MOSFET]);
                    PCA.setChannelOff(this->pins[PORT][HS_MOSFET]);                    
                    delayMicroseconds(500);       
                                              
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
            }  
        }        
    }   
}