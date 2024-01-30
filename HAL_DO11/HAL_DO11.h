#ifndef HAL_DO11_h
#define HAL_DO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "PCA9685.h"
#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
#include "BPLC_LOG.h"
#include "I2C_check.h"

//IO Channelstate
typedef enum
{
    DO_CHANNEL__NOT_USED,
    DO_CHANNEL__DIGITAL,
    DO_CHANNEL__ANALOG,
    DO_CHANNEL__SERVO,

    DO_CHANNEL__COUNT,
}e_DO_CHANNEL_STATE_t;

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
    DO11_CHANNEL__1,
    DO11_CHANNEL__2,
    DO11_CHANNEL__3,   
    DO11_CHANNEL__4,
    DO11_CHANNEL__5,
    DO11_CHANNEL__6,
    DO11_CHANNEL__7,
    DO11_CHANNEL__8,

    DO11_CHANNEL__COUNT,

}e_DO11_CHANNEL_t;


#define DEAD_TIME 100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung
#define LS_MOSFET 0
#define HS_MOSFET 1

class HAL_DO11 {

    public:
    HAL_DO11();
    void begin(const e_DO11_ADDRESS_t I2C_ADDRESS);

    e_BPLC_ERROR_t  mapObjectToChannel (Output*     P_OBJECT,  const e_DO11_CHANNEL_t CHANNEL);
    e_BPLC_ERROR_t  mapObjectToChannel (servoMotor* P_OBJECT,  const e_DO11_CHANNEL_t CHANNEL);

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
        e_DO_CHANNEL_STATE_t    state           [DO11_CHANNEL__COUNT];
        Output*                 p_outputInstance[DO11_CHANNEL__COUNT]; 
        servoMotor*             p_servoInstance [DO11_CHANNEL__COUNT];       
        const uint8_t           PIN             [DO11_CHANNEL__COUNT][2] = {{15, 4}, {14, 5}, {13, 6}, {12, 7}, {8, 0}, {9, 1}, {10, 2}, {11, 3}};     //{lowside, highside}
    }channels;  
};
#endif