#ifndef HAL_MCU11_h
#define HAL_MCU11_h
//---------------------------------------------------
/**
 * @file HAL_MCU11_revA.h
 * @author MIE
 * @brief 
 * Einlsen von Encoder
 * Setzten von LD 1-3
 * Setzen von p_oen
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
#include "BPLC_LOG.h"

//#define DEBUGGING_MCU11_revA    //p_oen und Buzzer pin wird verwedet für debugging
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
class HAL_MCU11_revA
{
    public:
    HAL_MCU11_revA();
    
    void begin          ();
    void mapEncoder     (rotaryEncoder* P_ENCODER);
    void mapBuzzer      (Output*        P_BUZZER_OBJECT);
    void mapLD1         (Output*        P_LD1_OBJECT);
    void mapLD2         (Output*        P_LD2_OBJECT);
    void mapLD3         (Output*        P_LD3_OBJECT);
    void mapOEN         (Output*        P_OEN_OBJECT);
    void mapINT         (uint8_t*       P_INT_COUNT);

    void tick();


    private:   
    rotaryEncoder*   p_encoder;
    Output*          p_buzzer;
    Output*          p_ld1;
    Output*          p_ld2;
    Output*          p_ld3;   
    Output*          p_oen;             

    //Serial Baudrate
    struct 
    {
        unsigned long USB   = 115200;
        unsigned long RS232 = 38400;
        unsigned long RS485 = 38400;
    }baudrate;    
    
    //Pinsdefinition
    struct 
    {        
        const uint8_t encoder_A = 39;
        const uint8_t encoder_B = 36;
        const uint8_t encoder_Z = 34;  
        const uint8_t ld1       = 27;
        const uint8_t ld2       = 26;
        const uint8_t ld3       = 25;
        #ifdef DEBUGGING_MCU11_revA
        const uint8_t p_oen       = 25;
        #endif
        #ifndef DEBUGGING_MCU11_revA
        const uint8_t p_oen       = 13;
        #endif
        const uint8_t INT       = 35;
        #ifdef DEBUGGING_MCU11_revA
        const uint8_t buzzer       = 25;
        #endif
        #ifndef DEBUGGING_MCU11_revA
        const uint8_t buzzer       = 15;
        #endif
    }PIN;   
};

class HAL_MCU11_revB
{
    public:
    HAL_MCU11_revB();
    
    void begin          ();
    void mapEncoder     (rotaryEncoder* P_ENCODER);
    void mapBuzzer      (Output*        P_BUZZER_OBJECT);
    void mapLD1         (Output*        P_LD1_OBJECT);
    void mapLD2         (Output*        P_LD2_OBJECT);
    void mapLD3         (Output*        P_LD3_OBJECT);
    void mapOEN         (Output*        P_OEN_OBJECT);
    void mapINT         (uint8_t*       P_INT_COUNT);

    void tick();


    private:
    rotaryEncoder*   p_encoder;
    Output*          p_buzzer;
    Output*          p_ld1;
    Output*          p_ld2;
    Output*          p_ld3;   
    Output*          p_oen;  
    //Serial Baudrate
    struct 
    {
        unsigned long USB   = 115200;
        unsigned long RS232 = 38400;
        unsigned long RS485 = 38400;
    }baudrate;    
    
    //Pinsdefinition
    struct 
    {        
        const uint8_t encoder_A = 39;
        const uint8_t encoder_B = 36;
        const uint8_t encoder_Z = 34;  
        const uint8_t ld1       = 27;
        const uint8_t ld2       = 26;
        const uint8_t ld3       = 25;
        const uint8_t OEN       = 2;   
        const uint8_t INT       = 35;
        const uint8_t buzzer    = 5;       
    }PIN;    
};
#endif