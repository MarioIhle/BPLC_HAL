#ifndef HAL_DI11_h
#define HAL_DI11_h

#include "Arduino.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE PORTS
//-------------------------------------------------------------
typedef enum
{
    DI11_1 = 0x20,
    DI11_2 = 0x21,
    DI11_3 = 0x22,
    DI11_4 = 0x23,
    
    DI11_COUNT = 4,

}e_DI11_ADDRESS_t;

//DIGITAL INPUTS
typedef enum
{
    DI1,
    DI2,
    DI3,
    DI4,
    DI5,
    DI6,
    DI7,
    DI8,

    DI_COUNT,

}e_digitalInputPorts_t;
//-------------------------------------------------------------
//SPEZIAL IO TYPEN
//-------------------------------------------------------------
//DigitalInput
typedef struct
{        
    bool state;
    bool previousState;
}s_digitalInputState_t;

//-------------------------------------------------------------
//HAL_DI11 KLASSE
//-------------------------------------------------------------
class HAL_DI11 {

    public:
            HAL_DI11    ();
            HAL_DI11    (const e_DI11_ADDRESS_t ADRESS);
    void    begin       (const e_DI11_ADDRESS_t ADRESS);
    void    tick        ();

    s_digitalInputState_t   digitalInputState [DI_COUNT];  

    protected:
    

    private:      
    uint8_t  deviceAdress;    
    };
#endif