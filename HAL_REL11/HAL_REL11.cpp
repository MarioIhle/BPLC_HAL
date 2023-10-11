#include "HAL_REL11.h"

HAL_REL11::HAL_REL11()
{}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1)
{
    this->p_REL[0] = P_REL1;    
    this->usedPortCount = 1;

    this->deviceAdress  = ADDRESS;
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;    
    this->usedPortCount = 2;

    this->deviceAdress  = ADDRESS;
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;
    this->p_REL[2] = P_REL3;
    this->usedPortCount = 3;

    this->deviceAdress  = ADDRESS;    
}

void HAL_REL11::begin()
{
    this->PCF.setAddress(this->deviceAdress);       //Tatsächliche Adresse schreiben
    this->PCF.begin();                              //Kommunikation hetstellen
    this->PCF.write8(false);                        //Alle Ports LOW

#ifdef DEBUG_HAL_REL11 
Serial.print("HAL_REL11.begin with I2C Address"); Serial.println(this->deviceAdress);
Serial.print("portCount: "); Serial.println(this->usedPortCount);
#endif
}

void HAL_REL11::tick()
{
    for(int PORT = 0; PORT < this->usedPortCount; PORT++)
    {
        if(this->p_REL[PORT]->isThereANewPortValue())   //Nur Wert abrufen und schreiben, falls dier sich geändert hat
        {
            const s_portValue_t VALUE_TO_WRITE = this->p_REL[PORT]->getValue();

            if(VALUE_TO_WRITE.value >= 1)
            {
                this->PCF.write(this->pins[PORT], true);
            }
            else if(VALUE_TO_WRITE.value == false)
            {
                this->PCF.write(this->pins[PORT], false);
            } 

#ifdef DEBUG_HAL_REL11 
Serial.print("value: "); Serial.println(VALUE_TO_WRITE.value);
Serial.print("previours value: "); Serial.println(VALUE_TO_WRITE.previousValue);
#endif

        }               
    }    
}