#include "HAL_DI11.h"

PCF8574 DI11_register(0x20);

HAL_DI11::HAL_DI11()
{}

void HAL_DI11::begin(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1; 

    this->usedPortCount = 5;    

    //DI11_register.begin(ADDRESS);                                   //Kommunikation hetstellen

    Serial.println("begin");  
    Serial.print("PORT_COUNT: "); Serial.print(this->usedPortCount);
    delay(5000);  
}
         

/*
HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->p_ports[DI_PORT_2] = P_PORT_2;

    uint8_t usedPortCount = 2;
   

    DI11_register.begin(ADDRESS);                                                    //Kommunikation hetstellen     
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;    

    uint8_t usedPortCount = 3;

    DI11_register.begin(ADDRESS);                                                    //Kommunikation hetstellen   
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;    

    uint8_t usedPortCount = 4;

    DI11_register.begin(ADDRESS);                                                  //Kommunikation hetstellen    
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;

    uint8_t usedPortCount = 5;
    
    DI11_register.begin(ADDRESS);                                                    //Kommunikation hetstellen    
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;

    uint8_t usedPortCount = 6;

    DI11_register.begin(ADDRESS);                                                    //Kommunikation hetstellen   
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
    this->p_ports[DI_PORT_7] = P_PORT_7;

    uint8_t usedPortCount = 7;
    
    DI11_register.begin(ADDRESS); 
}*/

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1 = nullptr, DigitalInput* P_PORT_2 = nullptr, DigitalInput* P_PORT_3 = nullptr, DigitalInput* P_PORT_4 = nullptr, DigitalInput* P_PORT_5 = nullptr, DigitalInput* P_PORT_6 = nullptr, DigitalInput* P_PORT_7 = nullptr, DigitalInput* P_PORT_8 = nullptr)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
    this->p_ports[DI_PORT_7] = P_PORT_7;
    this->p_ports[DI_PORT_8] = P_PORT_8; 

    uint8_t usedPortCount = 1;

    DI11_register.begin(ADDRESS);                                                    //Kommunikation hetstellen
}

void HAL_DI11::tick()
{Serial.print("PORT_COUNT: "); Serial.print(this->usedPortCount);
    for(uint8_t PORT = 0; PORT < this->usedPortCount; PORT++)
    {                 
        //this->p_ports[PORT]->setPortState(DI11_register.digitalRead(this->PINS[PORT]));             
        Serial.print(" PORT: "); Serial.print(DI11_register.read(this->PINS[PORT]));
    } 
}