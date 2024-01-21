#ifndef HAL_DO11_h
#define HAL_DO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "PCA9685.h"
#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "I2C_check.h"

typedef enum
{
    DO11_CARD_1_ADDRESS = 0x43,
    DO11_CARD_2_ADDRESS = 0x42,
    DO11_CARD_3_ADDRESS = 0x41,
    DO11_CARD_4_ADDRESS = 0x40,
    
    DO11_CARD_ADDRESS_COUNT = 4,

}e_DO11_ADDRESS_t;

typedef enum
{
    DO11_CARD__1,
    DO11_CARD__2,
    DO11_CARD__3,
    DO11_CARD__4,
    
    DO11_CARD__MAX = 4,

}e_DO11_CARD_t;

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

}e_DO11_PORTS_t;


#define DEAD_TIME 100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung
#define LS_MOSFET 0
#define HS_MOSFET 1

class HAL_DO11 {

    public:
    HAL_DO11();
    void begin(const e_DO11_ADDRESS_t I2C_ADDRESS);

    void            setPWMFreqency          (const uint8_t FREQUENCY)
    e_BPLC_ERROR_t  mapObjectToNextFreePort (Output* P_OBJECT);
    e_BPLC_ERROR_t  mapObjectToSpecificPort (Output* P_OBJECT, const e_DO11_PORTS_t PORT);

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
    
    struct
    {
        e_PORT_USEAGE_t used    [DO11_PORT__COUNT];
        Output*         p_object[DO11_PORT__COUNT];        
    }ports; 
    const uint8_t   PIN     [DO11_PORT__COUNT][2]= {{15, 4}, {14, 5}, {13, 6}, {12, 7}, {8, 0}, {9, 1}, {10, 2}, {11, 3}};     //{lowside, highside}     
};
#endif