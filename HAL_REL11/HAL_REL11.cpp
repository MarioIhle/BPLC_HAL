#include "HAL_REL11.h"

PCF8574 REL11_register(REL11_CARD_1);

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS)
{   
    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    REL11_register.write8(false);          //Alle Ports LOW
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1)
{
    this->p_REL[0] = P_REL1;    
    this->usedPortCount = 1;
 
    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    REL11_register.write8(false);          //Alle Ports LOW
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;    
    this->usedPortCount = 2;

    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    REL11_register.write8(false);          //Alle Ports LOW
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;
    this->p_REL[2] = P_REL3;
    this->usedPortCount = 3;

    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    REL11_register.write8(false);          //Alle Ports LOW
}

void HAL_REL11::tick()
{
    for(int PORT = 0; PORT < this->usedPortCount ; PORT++)
    {
        this->p_REL[PORT]->tick();
        const s_portValue_t     VALUE_TO_WRITE      = this->p_REL[PORT]->getValue();
        const bool              VALUE_HAS_CHANGED   = (bool)(VALUE_TO_WRITE.value != VALUE_TO_WRITE.previousValue);
      
        if(VALUE_HAS_CHANGED)
        {
            if(VALUE_TO_WRITE.value >= 1)
            {
                REL11_register.write(this->pins[PORT], true);
            }
            else if(VALUE_TO_WRITE.value == false)
            {
                REL11_register.write(this->pins[PORT], false);
            } 
        }               
    }    
}