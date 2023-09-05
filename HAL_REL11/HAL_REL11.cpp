#include "HAL_REL11.h"

PCF8574 REL11_register(REL11_CARD_1);

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS)
{   
    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    //  f_pcfOk = REL11_register.isConnected();           //mögliche Fehlererkennung
    REL11_register.write8(false);          //Alle Ports LOW
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1)
{
    this->p_REL[0] = P_REL1;    

    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    //  f_pcfOk = REL11_register.isConnected();           //mögliche Fehlererkennung
    REL11_register.write8(false);          //Alle Ports LOW
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;    

    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    //  f_pcfOk = REL11_register.isConnected();           //mögliche Fehlererkennung
    REL11_register.write8(false);          //Alle Ports LOW
}

HAL_REL11::HAL_REL11(const e_REL11_ADDRESS_t ADDRESS, Output* P_REL1, Output* P_REL2, Output* P_REL3)
{
    this->p_REL[0] = P_REL1;
    this->p_REL[1] = P_REL2;
    this->p_REL[2] = P_REL3;

    REL11_register.setAddress(ADDRESS);    //Tatsächliche Adresse schreiben
    REL11_register.begin();                //Kommunikation hetstellen
    //  f_pcfOk = REL11_register.isConnected();           //mögliche Fehlererkennung
    REL11_register.write8(false);          //Alle Ports LOW
}

void HAL_REL11::tick()
{
    for(int PORT = 0; PORT< 3; PORT++)
    {
        this->p_REL[PORT]->tick();
        if(this->p_REL[PORT]->getValue() == 255)
        {
            REL11_register.write(PORT, true);
        }
        else
        {
            REL11_register.write(PORT, false);
        }        
    }    
}