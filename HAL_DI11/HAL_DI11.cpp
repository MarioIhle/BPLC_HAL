#include "HAL_DI11.h"

HAL_DI11::HAL_DI11()
{
    Serial.begin(9600);
}
void HAL_DI11::begin()
{
    Serial.println("Hi_HAL");    
}

void HAL_DI11::tick()
{}