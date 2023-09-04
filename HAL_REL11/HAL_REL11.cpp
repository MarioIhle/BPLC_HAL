#include "HAL_REL11.h"

HAL_REL11::HAL_REL11(const uint8_t ADDRESS)
{
    this->cardAdress = ADDRESS;
}
HAL_REL11::HAL_REL11(const uint8_t ADDRESS, Output* REL1, Output* REL2, Output* REL3)
{

}

void HAL_REL11::begin(const uint8_t ADDRESS)
{
    this->cardAdress = ADDRESS;
}

void HAL_REL11::tick()
{
    for(int i = 0; i< 3; i++)
    {
        digitalWrite(this->pins[i], this->p_REL[i]->getValue());
    }    
}