#include "HAL_FUSE11.h"

HAL_FUSE11::HAL_FUSE11()
{}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->usedPortCount     = 1;

    this->deviceAdress  = ADDRESS;
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DO[OUT_PORT_2]  = P_DO2;
    this->usedPortCount     = 2;

    this->deviceAdress  = ADDRESS;
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DO[OUT_PORT_2]  = P_DO2;
    this->p_DO[OUT_PORT_3]  = P_DO3;  
    this->usedPortCount     = 3;

    this->deviceAdress  = ADDRESS;
}

HAL_FUSE11::HAL_FUSE11(const e_FUSE11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4)
{
    this->p_DO[OUT_PORT_1]  = P_DO1;
    this->p_DO[OUT_PORT_2]  = P_DO2;
    this->p_DO[OUT_PORT_3]  = P_DO3;
    this->p_DO[OUT_PORT_4]  = P_DO4;
    this->usedPortCount     = 4;

    this->deviceAdress  = ADDRESS;
}

void HAL_FUSE11::begin()
{
    this->PCF.setAddress(this->deviceAdress);       //Tatsächliche Adresse schreiben
    this->PCF.begin();                              //Kommunikation hetstellen
    this->PCF.write8(false);                        //Alle Ports LOW

    this->to_fuseCheck.setInterval(2500);
    
#ifdef DEBUG_HAL_FUSE11 
Serial.print("HAL_REL11.begin with I2C Address"); Serial.println(this->deviceAdress);
Serial.print("portCount: "); Serial.println(this->usedPortCount);
#endif
}

void HAL_FUSE11::tick()
{
     //INPUTs
    if(this->to_fuseCheck.check())
    {
        for(int PORT = 0; PORT < IN_PORT_COUNT ; PORT++)
        {       
            const bool STATE = PCF.read(this->pins.input[PORT]);     
            this->FUSE[PORT].setPortState(STATE);    
            f_somePinOfsomePinCardChanged--;  
#ifdef DEBUG_HAL_FUSE11
Serial.print(", PORT "); Serial.print(PORT); Serial.print(": "); Serial.print(STATE);       
#endif  
        }   
        this->to_fuseCheck.reset();
#ifdef DEBUG_HAL_FUSE11
Serial.println(" ");       
#endif  
    }

    for(int PORT = 0; PORT < this->usedPortCount; PORT++)
    {
        //OUTPUTs
        this->p_DO[PORT]->tick();   //Alle OUTs müssen getickt werden, damit ein Blinken möglich wird

        if(this->p_DO[PORT]->isThereANewPortValue())    //Nur Wert abrufen und schreiben, falls dier sich geändert hat
        {            
            const s_portValue_t VALUE_TO_WRITE = this->p_DO[PORT]->getValue();
                       
            if(VALUE_TO_WRITE.value >= 1)
            {
                PCF.write(this->pins.output[PORT], false);
            }
            else if(VALUE_TO_WRITE.value == false)
            {
                PCF.write(this->pins.output[PORT], true);
            }            
        }                      
    } 
}

uint8_t HAL_FUSE11::isAFuseBroken()
{
    uint8_t BROKEN_FUSE = 0;

    for(int FUSE = 0; FUSE < this->usedPortCount ; FUSE++)
    {
        if(this->FUSE[FUSE].islow())
        {
            BROKEN_FUSE = FUSE;
            break;
        }   
    }
#ifdef DEBUG_HAL_FUSE11
Serial.print("BROKEN_FUSE: "); Serial.println(BROKEN_FUSE);     
#endif  
    return BROKEN_FUSE;
}