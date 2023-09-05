#include "HAL_DI11.h"

PCF8574 DI11_register(DI11_CARD_1);

HAL_DI11::HAL_DI11()
{}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1; 

    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}
         
HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1; 
    this->p_ports[DI_PORT_2] = P_PORT_2;
    
    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;     

    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;    

    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    
    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
     

    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
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
    
    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}

HAL_DI11::HAL_DI11(const e_DI11_ADDRESS_t ADDRESS, DigitalInput* P_PORT_1, DigitalInput* P_PORT_2, DigitalInput* P_PORT_3, DigitalInput* P_PORT_4, DigitalInput* P_PORT_5, DigitalInput* P_PORT_6, DigitalInput* P_PORT_7, DigitalInput* P_PORT_8)
{    
    this->p_ports[DI_PORT_1] = P_PORT_1;
    this->p_ports[DI_PORT_2] = P_PORT_2;
    this->p_ports[DI_PORT_3] = P_PORT_3;
    this->p_ports[DI_PORT_4] = P_PORT_4;
    this->p_ports[DI_PORT_5] = P_PORT_5;
    this->p_ports[DI_PORT_6] = P_PORT_6;
    this->p_ports[DI_PORT_7] = P_PORT_7;
    this->p_ports[DI_PORT_8] = P_PORT_8; 

    DI11_register.setAddress(ADDRESS);                                        //Tatsächliche Adresse schreiben
    DI11_register.begin();                                                    //Kommunikation hetstellen
    //  f_pcfOk = DI11_register.isConnected();                                //mögliche Fehlererkennung
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)             //Alle Ports lesen
    {
        this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
    }     
}

void HAL_DI11::tick()
{
    for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)
    {             
        for(uint8_t PORT = 0; PORT < DI_PORT_COUNT; PORT++)          
        {
            this->p_ports[PORT]->setPortState(DI11_register.read(this->PINS[PORT]));
        }            
    } 
}