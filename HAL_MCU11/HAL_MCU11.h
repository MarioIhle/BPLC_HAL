#ifndef HAL_MCU11_h
#define HAL_MCU11_h
//---------------------------------------------------
/**
 * @file HAL_MCU11.h
 * @author MIE
 * @brief 
 * Einlsen von Encoder
 * Setzten von LD 1-3
 * Setzen von OEN
 * Lesen von INR???!
 * GPIOs
 * 
 * @version 1.0
 * @date 2022-12-19 * 
 */
//---------------------------------------------------

#include "Arduino.h"
#include "Wire.h"
#include "SpecialFunctions.h"
#include "BPLC_IOM.h"

#define revA
//#define revB

//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
//Encoder


typedef enum
{
    ENCODER_PIN__A,
    ENCODER_PIN__B,
    ENCODER_PIN__Z,

    ENCODER_PIN__COUNT,
}e_ENCODER_PINS_t;

//DigitalInput
typedef struct
{        
    bool state;
    bool previousState;
}s_digitalInputState_t_MCU;


//--------------------------------------------------------------------
//HAL KLASSE
//--------------------------------------------------------------------
class HAL_MCU11
{
    public:
    HAL_MCU11();
    
    void begin(void (*INT_callBack)(void));
    void tick();

    RotaryEncoder   ENCODER; 
    Output          BUZZER;
    Output          LD1;
    Output          LD2;
    Output          LD3;   
    Output          OEN;             

    private:
    //Serial Baudrate
    struct 
    {
        unsigned long USB   = 115200;
        unsigned long RS232 = 38400;
        unsigned long RS485 = 38400;
    }baudrate;    
    
    //Pinsdefinition
    #ifdef revA
    struct 
    {        
        const uint8_t encoder[3]= {39, 36, 34};
        const uint8_t led[3]    = {27, 26, 25};
        const uint8_t OEN       = 13;
        const uint8_t INT       = 35;
        const uint8_t buzzer    = 15;
    }PIN;
    #endif
    #ifdef revB
    struct 
    {        
        const uint8_t encoder[3]= {39, 36, 34};
        const uint8_t led[3]    = {27, 26, 25};
        const uint8_t OEN       = 2;
        const uint8_t INT       = 35;
        const uint8_t buzzer    = 5;
    }PIN;
    #endif

    DigitalInput    Encoder_A;
    DigitalInput    Encoder_B;
    DigitalInput    Encoder_Z;
};
#endif