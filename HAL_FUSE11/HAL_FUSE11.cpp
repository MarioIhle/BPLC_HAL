#include "HAL_FUSE11.h"

HAL_FUSE11::HAL_FUSE11()
{}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, DigitalInput* P_T_F1)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DI[IN_PORT_1]   = P_T_F1;
    this->usedPortCount     = 1;

    this->deviceAdress  = ADDRESS;
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, DigitalInput* P_T_F1, DigitalInput* P_T_F2)
{
     this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DI[IN_PORT_1]   = P_T_F1;
    this->p_DO[OUT_PORT_2]  = P_DO2;
    this->p_DI[IN_PORT_2]   = P_T_F2;   
    this->usedPortCount     = 2;

    this->deviceAdress  = ADDRESS;
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, DigitalInput* P_T_F1, DigitalInput* P_T_F2, DigitalInput* P_T_F3)
{
     this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DI[IN_PORT_1]   = P_T_F1;
    this->p_DO[OUT_PORT_2]  = P_DO2;
    this->p_DI[IN_PORT_2]   = P_T_F2;
    this->p_DO[OUT_PORT_3]  = P_DO3;
    this->p_DI[IN_PORT_3]   = P_T_F3;    
    this->usedPortCount     = 3;

    this->deviceAdress  = ADDRESS;
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, DigitalInput* P_T_F1, DigitalInput* P_T_F2, DigitalInput* P_T_F3, DigitalInput* P_T_F4)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DI[IN_PORT_1]   = P_T_F1;
    this->p_DO[OUT_PORT_2]  = P_DO2;
    this->p_DI[IN_PORT_2]   = P_T_F2;
    this->p_DO[OUT_PORT_3]  = P_DO3;
    this->p_DI[IN_PORT_3]   = P_T_F3;
    this->p_DO[OUT_PORT_4]  = P_DO4;
    this->p_DI[IN_PORT_4]   = P_T_F4;
    this->usedPortCount     = 4;

    this->deviceAdress  = ADDRESS;
}

void HAL_FUSE11::begin()
{
    this->PCF.setAddress(this->deviceAdress);       //Tatsächliche Adresse schreiben
    this->PCF.begin();                              //Kommunikation hetstellen
    
#ifdef DEBUG_HAL_FUSE11 
Serial.print("HAL_REL11.begin with I2C Address"); Serial.println(this->deviceAdress);
Serial.print("portCount: "); Serial.println(this->usedPortCount);
#endif
}

void HAL_FUSE11::tick()
{
    for(int PORT = 0; PORT < this->usedPortCount ; PORT++)
    {
        //INPUTs
        if(this->f_somePinOfsomePinCardChanged >0)
        {
            const bool STATE = !PCF.read(this->pins.input[PORT]);     
            this->p_DI[PORT]->setPortState(STATE);    
            f_somePinOfsomePinCardChanged--;       
        }

        //OUTPUTs
        this->p_DO[PORT]->tick();   //Alle OUTs müssen getickt werden, damit ein Blinken möglich wird

        if(this->p_DO[PORT]->isThereANewPortValue())    //Nur Wert abrufen und schreiben, falls dier sich geändert hat
        {            
            const s_portValue_t VALUE_TO_WRITE = this->p_DO[PORT]->getValue();
                       
            if(VALUE_TO_WRITE.value >= 1)
            {
                PCF.write(this->pins.output[PORT], true);
            }
            else if(VALUE_TO_WRITE.value == false)
            {
                PCF.write(this->pins.output[PORT], false);
            }            
        }                      
    }    
}

void HAL_FUSE11::somePinOfsomePinCardChanged()
{
    this->f_somePinOfsomePinCardChanged = 2;
}