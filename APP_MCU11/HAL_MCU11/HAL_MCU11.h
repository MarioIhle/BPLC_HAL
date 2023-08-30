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
//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
//Encoder
typedef enum
{
    idle,
    left,
    right,
}e_direction_t;

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
}s_digitalInputState_t;


//--------------------------------------------------------------------
//HAL KLASSE
//--------------------------------------------------------------------
class HAL_MCU11
{
    public:
    HAL_MCU11();
    
    void begin();
    void tick();

    e_direction_t   getEncoderDirection();
    bool            isEncoderButtonPressed();
    void            setOEN(const bool STATE);
    bool            getINT();


    private:

    //Serial Baudrate
    struct 
    {
        uint16_t USB   = 115200;
        uint16_t RS232 = 38400;
        uint16_t RS485 = 38400;
    }baudrate;
    

    //Portstates
    struct
    {
         struct 
        {
            s_digitalInputState_t  A;
            s_digitalInputState_t  B;
            s_digitalInputState_t  Z;
        }encoder;

        struct 
        {
            bool LD1;
            bool LD2;
            bool LD3;
        }led;

        bool OEN;
        bool INT;        
    }ioState;

    //Pinsdefinition
    struct 
    {
        struct 
        {
            const uint8_t A = 39;
            const uint8_t B = 36;
            const uint8_t Z = 34;
        }encoder;

        struct 
        {
            const uint8_t LD1 = 27;
            const uint8_t LD2 = 26;
            const uint8_t LD3 = 25;
        }led;

        const uint8_t OEN = 13;
        const uint8_t INT = 35;
    }pins;
};
#endif