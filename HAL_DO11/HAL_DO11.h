#ifndef HAL_DO11_h
#define HAL_DO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "IOM_Base.h"
#include "PCA9685.h"

typedef enum
{
    DO11_CARD_1 = 0x43,
    DO11_CARD_2 = 0x42,
    DO11_CARD_3 = 0x41,
    DO11_CARD_4 = 0x40,
    
    DO11_CARD_COUNT = 4,

}e_DO11_ADDRESS_t;


#define LS_MOSFET 0
#define HS_MOSFET 1

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

#define DEAD_TIME   100 //besser geht nicht ohne kurzeitige KS bei PWM änderung

class HAL_DO11 {

    public:
    HAL_DO11();
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6, Output* P_DO7);
    HAL_DO11(const e_DO11_ADDRESS_t ADDRESS, Output* P_DO1, Output* P_DO2, Output* P_DO3, Output* P_DO4, Output* P_DO5, Output* P_DO6, Output* P_DO7, Output* P_DO8);
    
    void begin();
    void tick();
    
    private:
    PCA9685 PCA;

    Output*         p_DO    [DO_PORT_COUNT];
    const uint8_t   pins    [DO_PORT_COUNT][2]= {{15, 4}, {14, 5}, {13, 6}, {12, 7}, {8, 0}, {9, 1}, {10, 2}, {11, 3}};     //{lowside, highside}
                                            //  {15, 7}, {14, 6}, {13, 5}, {12, 4}, {8, 3}, {9, 2}, {10, 1}, {11, 0}
    uint8_t             usedPortCount;
    e_DO11_ADDRESS_t    deviceAddress;
};
#endif