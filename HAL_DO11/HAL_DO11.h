#ifndef HAL_DO11_h
#define HAL_DO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "IOM_Base.h"
#include "PCA9685.h"

typedef enum
{
    DO11_CARD_1 = 0x50,
    DO11_CARD_2 = 0x51,
    DO11_CARD_3 = 0x52,
    DO11_CARD_4 = 0x53,
    
    DO11_CARD_COUNT = 4,

}e_DO11_ADDRESS_t;

typedef enum
{
    DO_PORT_1,
    DO_PORT_2,
    DO_PORT_3,   
    DO_PORT_4,
    DO_PORT_5,
    DO_PORT_6,
    DO_PORT_7,
    DO_PORT_8,

    DO_PORT_COUNT,

}e_digitalOutputPorts_t;

typedef struct 
{
    uint8_t lowside;
    uint8_t highside;
}s_outPutState_t;

class HAL_DO11 {

    public:
    HAL_DO11();
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1);
    
    void        tick();
    
    private:   
    void writeOutputs();

    Output*         p_DO    [DO_PORT_COUNT];
    const uint8_t   pins    [DO_PORT_COUNT][2]= {{15, 7}, {14, 6}, {13, 5}, {12, 4}, {8, 3}, {9, 2}, {10, 1}, {11, 0}};     //{lowside, highside}

    s_outPutState_t portStates[DO_PORT_COUNT];      

    Timeout to_deadTime;
    Timeout to_pwmFrequence;
};


#endif