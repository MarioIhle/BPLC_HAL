#ifndef HAL_REL11_h
#define HAL_DI11_h
#include "Arduino.h"
//-------------------------------------------------------------
//HAL_IO KLASSE
//-------------------------------------------------------------
#define INPUT_READ_COUNT 2

class HAL_REL11 {

    public:
    HAL_REL11();
    void        begin();
    void        tick();
    
    bool    relState[3];

    private:
    void    setREl();
    uint8_t pins[3];    
};

#endif