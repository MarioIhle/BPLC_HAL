#include "HAL_FUSE11.h"

PCF8574 FUSE11_register(FUSE11_CARD_1);

HAL_FUSE11::HAL_FUSE11()
{}
HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, DigitalInput* P_T_F1)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DI[IN_PORT_1]   = P_T_F1;
    this->usedPortCount     = 1;

    FUSE11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    FUSE11_register.begin();                //Kommunikation hetstellen
    FUSE11_register.write8(false);          //Alle Ports LOW
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, DigitalInput* P_T_F1, DigitalInput* P_T_F2)
{}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, DigitalInput* P_T_F1, DigitalInput* P_T_F2, DigitalInput* P_T_F3)
{}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, DigitalInput* P_T_F1, DigitalInput* P_T_F2, DigitalInput* P_T_F3, DigitalInput* P_T_F4)
{}

void HAL_FUSE11::tick()
{
    for(int PORT = 0; PORT < this->usedPortCount ; PORT++)
    {
        //Testeingänge lesen und in DigitalInput objekte übergeben
        this->p_DI[PORT]->setPortState(FUSE11_register.read(this->pins.input[PORT]));          
        
        //Outputs setzten
        this->p_DO[PORT]->tick();
        const s_portValue_t     VALUE_TO_WRITE      = this->p_DO[PORT]->getValue();
        const bool              VALUE_HAS_CHANGED   = (bool)(VALUE_TO_WRITE.value != VALUE_TO_WRITE.previousValue);

        if(VALUE_HAS_CHANGED)
        {
            if(VALUE_TO_WRITE.value >= 1)
            {
                FUSE11_register.write(this->pins.output[PORT], true);
            }
            else if(VALUE_TO_WRITE.value == false)
            {
                FUSE11_register.write(this->pins.output[PORT], false);
            } 
        }               
    }    
}

   
