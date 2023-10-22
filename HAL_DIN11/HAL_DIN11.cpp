#include "HAL_DIN11.h"

#define READ_TWO_TIMES 2


HAL_DIN11::HAL_DIN11()
{
    this->usedPortCount = 0;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->usedPortCount = 1;
}
         
HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2)
{   
    this->deviceAddress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->usedPortCount = 2;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;    
    this->usedPortCount = 3;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;    
    this->usedPortCount = 4;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->usedPortCount = 5;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6)
{    
    this->deviceAddress = ADDRESS;

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
    this->deviceAddress = ADDRESS;

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
    this->deviceAddress = ADDRESS;

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

e_BPLC_ERROR_t HAL_DIN11::begin()
{    
    e_BPLC_ERROR_t error = BPLC_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup DIN11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case DIN11_CARD_1:
            Serial.println("1");
        break;
        case DIN11_CARD_2:
            Serial.println("2");
        break;
        case DIN11_CARD_3:
            Serial.println("3");
        break;
        case DIN11_CARD_4:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);
    
    Serial.print("Ports defined: "); Serial.print(this->usedPortCount); Serial.println("/8");
 
    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = BPLC_ERROR__DIN11_COMMUNICATION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(error == BPLC_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();    

        this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;        
        this->f_error = false;  
    }
    else
    {
        this->f_error = true;                      
    }

    return error;
}

void HAL_DIN11::begin(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1 = nullptr, DigitalInput* P_PORT_2 = nullptr, DigitalInput* P_PORT_3 = nullptr, DigitalInput* P_PORT_4 = nullptr, DigitalInput* P_PORT_5 = nullptr, DigitalInput* P_PORT_6 = nullptr, DigitalInput* P_PORT_7 = nullptr, DigitalInput* P_PORT_8 = nullptr)
{  
    this->deviceAddress = ADDRESS;

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

void HAL_DIN11::tick()
{      
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->f_error = true;
    }
    else
    {
        //this->f_error = false;    //selbrücksetellung des Fehlerzustands, zur zeit nicht genutzt
    }

    if(!this->f_error)
    {           
        if(this->f_somePinOfsomePinCardChanged > 0)
        {
            #ifdef DEBUG_HAL_DIN11
            Serial.print("DIN11 "); Serial.print(this->deviceAddress); Serial.println("STATES:");
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

e_BPLC_ERROR_t HAL_DIN11::getError()
{
    e_BPLC_ERROR_t tempError = BPLC_ERROR__NO_ERROR;
    if(this->f_error)
    {
        tempError = BPLC_ERROR__DIN11_COMMUNICATION_FAILED;
    }
    return tempError;
}