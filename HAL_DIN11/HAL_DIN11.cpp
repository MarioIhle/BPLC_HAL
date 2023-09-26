#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11()
{
    this->usedPortCount = 0;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->usedPortCount = 1;
}
         
HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2)
{   
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->usedPortCount = 2;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;    
    this->usedPortCount = 3;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;    
    this->usedPortCount = 4;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->usedPortCount = 5;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6)
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

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7)
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

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1 = nullptr, DigitalInput* P_PORT_2 = nullptr, DigitalInput* P_PORT_3 = nullptr, DigitalInput* P_PORT_4 = nullptr, DigitalInput* P_PORT_5 = nullptr, DigitalInput* P_PORT_6 = nullptr, DigitalInput* P_PORT_7 = nullptr, DigitalInput* P_PORT_8 = nullptr)
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

void HAL_DIN11::begin()
{    
    PCF.setAddress(this->deviceAdress);   //Tatsächliche Adresse schreiben
    PCF.begin();                          //Kommunikation hetstellen 

    this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;       //inital Ports lesen, bei nächsten Tick() aufruf
    
#ifdef DEBUG_HAL_DIN11 
Serial.print("HAL_REL11.begin with I2C Address"); Serial.println(this->deviceAdress);
Serial.print("portCount: "); Serial.println(this->usedPortCount);
#endif
}

void HAL_DIN11::tick()
{    
    if(this->f_somePinOfsomePinCardChanged > 0)
    {
#ifdef DEBUG_HAL_DIN11
Serial.print("DIN11 "); Serial.print(this->deviceAdress); Serial.println("STATES:");
#endif
        for(uint8_t PORT = 0; PORT < this->usedPortCount; PORT++)
        {            
            const bool STATE = !PCF.read(this->PINS[PORT]);     
            this->p_ports[PORT]->setPortState(STATE);   

#ifdef DEBUG_HAL_DIN11
Serial.print(", PORT "); Serial.print(PORT); Serial.print(": "); Serial.print(STATE);       
#endif
        } 
#ifdef DEBUG_HAL_DIN11
Serial.println(" ");       
#endif
        this->f_somePinOfsomePinCardChanged--;
    }    
}

void HAL_DIN11::somePinOfsomeDinCardChanged()
{
    this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;
}