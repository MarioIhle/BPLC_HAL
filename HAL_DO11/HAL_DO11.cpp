#include "HAL_DO11.h"

PCA9685 pwmRegister(DO11_CARD_1);

HAL_DO11::HAL_DO11()
{}

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1)
{
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

    this->p_DO[DO_PORT_1] = P_DO1;    

    this->usedPortCount = 1;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2)
{
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    
    this->usedPortCount = 2;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3)
{
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    
    this->usedPortCount = 3;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4)
{
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    
    this->usedPortCount = 4;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5)
{
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

    this->p_DO[DO_PORT_1] = P_DO1;    
    this->p_DO[DO_PORT_2] = P_DO2; 
    this->p_DO[DO_PORT_3] = P_DO3; 
    this->p_DO[DO_PORT_4] = P_DO4; 
    this->p_DO[DO_PORT_5] = P_DO5; 
    
    this->usedPortCount = 5;
}   

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6)
{
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

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
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

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
    pwmRegister.setI2CAddress(ADDRESS);
    pwmRegister.init();
    pwmRegister.setPWMFrequency();

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

void HAL_DO11::tick()
{
    for(uint8_t PORT; PORT < this->usedPortCount; PORT++)
    {        
        this->p_DO[PORT]->tick();
        //PWM von 0-255 laden und umrechnen
        const uint16_t TARGET_PWM_VALUE = map(this->p_DO[PORT]->getValue(), 0, 255, 0, 4096);
        const uint16_t OFF_TIME         = 4096 - TARGET_PWM_VALUE; 

        if(this->portStates[PORT] != TARGET_PWM_VALUE)
        {
            switch(p_DO[PORT]->getOutputType())
            {
                case OUTPUTTYPE__OPEN_DRAIN:
                    pwmRegister.setChannelPWM(this->pins[PORT][LS_MOSFET], 0, TARGET_PWM_VALUE);        //lowSide
                    pwmRegister.setChannelOff(this->pins[PORT][HS_MOSFET]);                             //highside
                break;

                case OUTPUTTYPE__OPEN_SOURCE:
                    pwmRegister.setChannelOff(this->pins[PORT][LS_MOSFET]);                             //lowSide
                    pwmRegister.setChannelPWM(this->pins[PORT][HS_MOSFET], 0, TARGET_PWM_VALUE);        //highside
                break;

                case OUTPUTTYPE__PUSH_PULL:                            
                    
                    //FULL OFF
                    if(TARGET_PWM_VALUE < DEAD_TIME)
                    {
                        pwmRegister.setChannelOn(this->pins[PORT][LS_MOSFET]);
                        pwmRegister.setChannelOff(this->pins[PORT][HS_MOSFET]);    
                    }
                    //FULL ON
                    else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                    {
                        pwmRegister.setChannelOff(this->pins[PORT][LS_MOSFET]);
                        pwmRegister.setChannelOn(this->pins[PORT][HS_MOSFET]);    
                    }
                    //PWM
                    else
                    {                        
                        pwmRegister.setChannelPWM(this->pins[PORT][LS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       OFF_TIME);
                        pwmRegister.setChannelPWM(this->pins[PORT][HS_MOSFET],  OFF_TIME + DEAD_TIME,               TARGET_PWM_VALUE);  
                    }
                break;
            }  
            this->portStates[PORT] == TARGET_PWM_VALUE;
        }        
    }   
}