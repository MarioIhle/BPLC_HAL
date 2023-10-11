#include "HAL_DIN11.h"

#define READ_TWO_TIMES 2


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

e_APP_ERROR_t HAL_DIN11::begin()
{    
    e_APP_ERROR_t error = APP_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.print("setup DIN11 CARD ");
    switch(this->deviceAdress)
    {
        case DIN11_CARD_1:
            Serial.print("1");
        break;
        case DIN11_CARD_2:
            Serial.print("2");
        break;
        case DIN11_CARD_3:
            Serial.print("3");
        break;
        case DIN11_CARD_4:
            Serial.print("4");
        break;
    }
    Serial.println("/4");
    Serial.print("Ports defined: "); Serial.print(this->usedPortCount); Serial.println("/8");
 
    //I2C Initialisieren
    PCF.setAddress(this->deviceAdress);   
    PCF.begin(); 
    Serial.print("I2C address: 0x"); Serial.println(this->deviceAdress, HEX); 
    Wire.beginTransmission(this->deviceAdress);
    const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0);
    	   
    if(DEVICE_FOUND)
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = APP_ERROR__DIN11_COMMUNICATION_FAILED;
        
    }

    //Applikationsparameter initialisieren
    this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;        
    if(error != APP_ERROR__NO_ERROR)
    {        
        this->f_error = true;
    }

    return error;
}

void HAL_DIN11::tick()
{   
    if(!f_error)
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
}

void HAL_DIN11::somePinOfsomeDinCardChanged()
{
    this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;
}