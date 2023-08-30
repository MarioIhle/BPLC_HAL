#include "HAL_DI11.h"

HAL_DI11::HAL_DI11()
{}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADRESS)
{
    //PCF.begin(ADRESS);

    for(uint8_t pin = 0; pin<DI_COUNT; pin++)
    {
        pinMode(pin, INPUT);
    }    
}

void HAL_DI11::begin(const e_DI11_ADDRESS_t ADRESS)
{
    //PCF.begin(ADRESS);

    for(uint8_t pin = 0; pin<DI_COUNT; pin++)
    {
        pinMode(pin, INPUT);
    } 
}

void HAL_DI11::tick()
{
    for(uint8_t pin = 0; pin < DI_COUNT; pin++)
    {
        this->digitalInputState[pin].previousState = this->digitalInputState[pin].state;
        this->digitalInputState[pin].state = digitalRead(pin);
    } 
}