#ifndef HAL_DO11_h
#define HAL_DO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "IOM_Base.h"
#include "PCA9685.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "I2C_check.h"

typedef enum
{
    DO11_CARD__1,
    DO11_CARD__2,
    DO11_CARD__3,
    DO11_CARD__4,
    
    DO11_CARD__COUNT = 4,

}e_DO11_CARD_t;

typedef enum
{
    DO11_CARD_1_ADDRESS = 0x43,
    DO11_CARD_2_ADDRESS = 0x42,
    DO11_CARD_3_ADDRESS = 0x41,
    DO11_CARD_4_ADDRESS = 0x40,
    
    DO11_CARD_ADDRESS_COUNT = 4,

}e_DO11_ADDRESS_t;

#define LS_MOSFET 0
#define HS_MOSFET 1

typedef enum
{
    DO11_PORT__1,
    DO11_PORT__2,
    DO11_PORT__3,   
    DO11_PORT__4,
    DO11_PORT__5,
    DO11_PORT__6,
    DO11_PORT__7,
    DO11_PORT__8,

    DO11_PORT__COUNT,

}e_DO11_PORTs_t;

#define DEAD_TIME   100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung

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
    void begin(const e_DO11_ADDRESS_t I2C_ADDRESS);
    
    void mapObjectToPort(Output* P_OBJECT);

    void tick();

    e_BPLC_ERROR_t getError();
    
    private:
    //Applikation

    //Safety
    I2C_check       selfCheck;
    e_BPLC_ERROR_t  errorCode;  

    //Settings
    PCA9685             PCA;
    e_DO11_ADDRESS_t    deviceAddress;
    uint8_t             usedPorts;

    Output*         p_ports [DO11_PORT__COUNT];
    const uint8_t   pins    [DO11_PORT__COUNT][2]= {{15, 4}, {14, 5}, {13, 6}, {12, 7}, {8, 0}, {9, 1}, {10, 2}, {11, 3}};     //{lowside, highside}  
};
#endif