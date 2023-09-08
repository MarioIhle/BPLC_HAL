#include "HAL_DI11.h"

HAL_DI11::HAL_DI11()
{
    this->usedPortCount = 0;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->usedPortCount = 1;
}
         
HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2)
{   
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->usedPortCount = 2;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;    
    this->usedPortCount = 3;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;    
    this->usedPortCount = 4;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->usedPortCount = 5;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
    this->usedPortCount = 6;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
    this->p_ports[DI_PORT_7] = P_PORT_7;
    this->usedPortCount = 7;
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1 = nullptr, DigitalInput* P_PORT_2 = nullptr, DigitalInput* P_PORT_3 = nullptr, DigitalInput* P_PORT_4 = nullptr, DigitalInput* P_PORT_5 = nullptr, DigitalInput* P_PORT_6 = nullptr, DigitalInput* P_PORT_7 = nullptr, DigitalInput* P_PORT_8 = nullptr)
{  
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
    this->p_ports[DI_PORT_7] = P_PORT_7;
    this->p_ports[DI_PORT_8] = P_PORT_8; 
    this->usedPortCount = 8;
}

void HAL_DI11::begin()
{    
    PCF.setAddress(this->deviceAdress);                                        //Tatsächliche Adresse schreiben
    PCF.begin();                                                    //Kommunikation hetstellen 
}

void HAL_DI11::tick()
{    
    if(this->f_somePinOfsomeDI11CardChanged >0)
    {
#ifdef DEBUG_HAL_DI11
Serial.print("DI11 "); Serial.print(this->deviceAdress); Serial.println("STATES:");
#endif
        for(uint8_t PORT = 0; PORT < this->usedPortCount; PORT++)
        {            
            const bool STATE = !PCF.read(this->PINS[PORT]);     
            this->p_ports[PORT]->setPortState(STATE);   

#ifdef DEBUG_HAL_DI11
Serial.print(", PORT "); Serial.print(PORT); Serial.print(": "); Serial.print(STATE);       
#endif
        } 
#ifdef DEBUG_HAL_DI11
Serial.println(" ");       
#endif
        this->f_somePinOfsomeDI11CardChanged--;
    }    
}

void HAL_DI11::somePinOfsomeDI11CardChanged()
{
    this->f_somePinOfsomeDI11CardChanged = 2;
}