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
#include "SpecialFunctions.h"

//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
//Encoder
typedef enum
{
    MCU_idle,
    MCU_left,
    MCU_right,
}e_direction_t_MCU;

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
    
    void begin();
    void tick();

    e_direction_t_MCU   getEncoderDirection     ();
    bool                isEncoderButtonPressed  ();
    void                setOEN                  (const bool STATE);
    bool                getINT                  ();
    blink               BUZZER;
    blink               LD1;
    blink               LD2;
    blink               LD3;                  

    private:

    //Serial Baudrate
    struct 
    {
        unsigned long USB   = 115200;
        unsigned long RS232 = 38400;
        unsigned long RS485 = 38400;
    }baudrate;
    

    //Portstates
    struct
    {
        s_digitalInputState_t_MCU encoder [ENCODER_PIN__COUNT];        
        bool OEN;
        bool INT;           
    }ioState;

    //Pinsdefinition
    struct 
    {        
        const uint8_t encoder[3]= {39, 36, 34};
        const uint8_t led[3]    = {27, 26, 25};
        const uint8_t OEN       = 13;
        const uint8_t INT       = 35;
        const uint8_t buzzer    = 15;
    }pins;
};
#endif