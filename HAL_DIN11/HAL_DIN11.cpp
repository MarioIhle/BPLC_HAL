#include "HAL_DIN11.h"

#define READ_TWO_TIMES 2

HAL_DIN11::HAL_DIN11()
{
    this->usedPorts = 0;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;

    this->usedPorts = 1;
}
         
HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2)
{   
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    
    this->usedPorts = 2;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    this->p_ports[DIN11_PORT__3] = P_PORT_3;
        
    this->usedPorts = 3;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    this->p_ports[DIN11_PORT__3] = P_PORT_3;
    this->p_ports[DIN11_PORT__4] = P_PORT_4;
 
    this->usedPorts = 4;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    this->p_ports[DIN11_PORT__3] = P_PORT_3;
    this->p_ports[DIN11_PORT__4] = P_PORT_4;
    this->p_ports[DIN11_PORT__5] = P_PORT_5;

    this->usedPorts = 5;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    this->p_ports[DIN11_PORT__3] = P_PORT_3;
    this->p_ports[DIN11_PORT__4] = P_PORT_4;
    this->p_ports[DIN11_PORT__5] = P_PORT_5;
    this->p_ports[DIN11_PORT__6] = P_PORT_6;
    this->usedPorts = 6;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    this->p_ports[DIN11_PORT__3] = P_PORT_3;
    this->p_ports[DIN11_PORT__4] = P_PORT_4;
    this->p_ports[DIN11_PORT__5] = P_PORT_5;
    this->p_ports[DIN11_PORT__6] = P_PORT_6;
    this->p_ports[DIN11_PORT__7] = P_PORT_7;
    this->usedPorts = 7;
}

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1 = nullptr, DigitalInput* P_PORT_2 = nullptr, DigitalInput* P_PORT_3 = nullptr, DigitalInput* P_PORT_4 = nullptr, DigitalInput* P_PORT_5 = nullptr, DigitalInput* P_PORT_6 = nullptr, DigitalInput* P_PORT_7 = nullptr, DigitalInput* P_PORT_8 = nullptr)
{  
    this->deviceAddress = ADDRESS;

    this->p_ports[DIN11_PORT__1] = P_PORT_1;
    this->p_ports[DIN11_PORT__2] = P_PORT_2;
    this->p_ports[DIN11_PORT__3] = P_PORT_3;
    this->p_ports[DIN11_PORT__4] = P_PORT_4;
    this->p_ports[DIN11_PORT__5] = P_PORT_5;
    this->p_ports[DIN11_PORT__6] = P_PORT_6;
    this->p_ports[DIN11_PORT__7] = P_PORT_7;
    this->p_ports[DIN11_PORT__8] = P_PORT_8;
    this->usedPorts = 8;
}

void HAL_DIN11::begin()
{    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup DIN11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case DIN11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case DIN11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case DIN11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case DIN11_CARD_4_ADDRESS:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);
    
    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/8");
 
    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == DIN11_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();    

        this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;        
    }
}

void HAL_DIN11::begin(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{  
    this->deviceAddress = I2C_ADDRESS;

    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup DIN11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case DIN11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case DIN11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case DIN11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case DIN11_CARD_4_ADDRESS:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);
    
    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/8");
 
    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == DIN11_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();    

        this->f_somePinOfsomePinCardChanged = READ_TWO_TIMES;        
    }
}

void HAL_DIN11::mapObjectToPort(DigitalInput* P_PORT)
{  
    this->p_ports[usedPorts] = P_PORT;
    this->usedPorts++;

    //Plausibilitätsprüfung
    if(this->usedPorts < DIN11_PORT__COUNT)
    {
        this->errorCode = DIN11_ERROR__PORT_OVERFLOW;
    }
}

void HAL_DIN11::tick()
{      
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = DIN11_ERROR__I2C_CONNECTION_FAILED;
    }
    else
    {
        //this->f_error = false;    //selbrücksetellung des Fehlerzustands, zur zeit nicht genutzt
    }

    if(this->errorCode == DIN11_ERROR__NO_ERROR)
    {           
        if(this->f_somePinOfsomePinCardChanged > 0)
        {
            #ifdef DEBUG_HAL_DIN11
            Serial.print("DIN11 "); Serial.print(this->deviceAddress); Serial.println("STATES:");
            #endif

            for(uint8_t PORT = 0; PORT < this->usedPorts; PORT++)
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

e_DIN11_ERROR_t HAL_DIN11::getError()
{
    return this->errorCode;
}