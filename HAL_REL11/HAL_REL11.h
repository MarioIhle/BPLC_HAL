#ifndef HAL_REL11_h
#define HAL_DI11_h
#include "Arduino.h"
#include "IOM_Base.h"

//-------------------------------------------------------------
//HAL_REL11 KLASSE
//-------------------------------------------------------------
class HAL_REL11 {

    public:
    HAL_REL11(const uint8_t ADDRESS);
    HAL_REL11(const uint8_t ADDRESS, Output* REL1, Output* REL2, Output* REL3);
    void        begin(const uint8_t ADDRESS);
    void        tick();
    
    private:
    uint8_t cardAdress;
    Output* p_REL[3];
    uint8_t pins[3] = {0,1,2};    
};

#endif