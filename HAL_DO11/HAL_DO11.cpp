#include "HAL_DO11.h"

PCA9685 pwmRegister;

HAL_DO11::HAL_DO11()
{}

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1)
{
    pwmRegister.init(ADDRESS);

    this->p_DO[DO_PORT_1] = P_DO1;
    this->to_pwmFrequence.setInterval(5);
    this->to_deadTime.setInterval(1);
}   

void HAL_DO11::tick()
{
    for(uint8_t PORT; PORT< DO_PORT_COUNT; PORT++)
    {
        switch(p_DO[PORT]->getOutputType())
        {
            case OUTPUTTYPE__OPEN_DRAIN:
                this->portStates[PORT].lowside = this->p_DO[PORT]->getValue(); 
                this->portStates[PORT].highside = 0; 
            break;

            case OUTPUTTYPE__OPEN_SOURCE:      
                this->portStates[PORT].lowside = 0;               
                this->portStates[PORT].highside = this->p_DO[PORT]->getValue();                                
            break;

            case OUTPUTTYPE__PUSH_PULL:                
                //FULL ON
                if(this->p_DO[PORT]->getValue() == 255)//== HIGH
                {
                    this->portStates[PORT].highside = 255;
                    this->portStates[PORT].lowside  = 0;
                }//FULL OFF
                else if(this->p_DO[PORT]->getValue() == 0)
                {
                    this->portStates[PORT].highside = 0;
                    this->portStates[PORT].lowside  = 255;
                }
                else
                {
                    if(this->to_pwmFrequence.check())
                    {
                        const uint8_t   TARGET_PWM_VALUE    = this->p_DO[PORT]->getValue();
                        const bool      HIGH_SIDE_IS_ACTIVE = (bool)(this->portStates[PORT].highside != 0);

                        if(HIGH_SIDE_IS_ACTIVE)
                        {
                            this->portStates[PORT].highside = 0;
                            this->portStates[PORT].lowside  = TARGET_PWM_VALUE;
                        }    
                        else
                        {
                            this->portStates[PORT].highside = TARGET_PWM_VALUE;
                            this->portStates[PORT].lowside  = 0;
                        }
                        this->to_pwmFrequence.reset();
                        this->to_deadTime.reset();
                    }                                                          
                }                                
            break;

        }
    }
    this->writeOutputs();
}

void HAL_DO11::writeOutputs()
{
    for(uint8_t PORT; PORT< DO_PORT_COUNT; PORT++)
    {
        if(this->to_deadTime.check())
        {
            //pins setzen        
            pwmRegister.setPwm(this->pins[PORT][0], 0, this->portStates[PORT].lowside);     //lowSide
            pwmRegister.setPwm(this->pins[PORT][1], 0, this->portStates[PORT].highside);    //highside
        }
        else
        {
            pwmRegister.setPwm(this->pins[PORT][0], 0, 0);  //lowside
            pwmRegister.setPwm(this->pins[PORT][1], 0, 0);  //highside
        }
    }    
}
