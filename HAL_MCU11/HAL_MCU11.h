#ifndef HAL_MCU11_h
#define HAL_MCU11_h
//---------------------------------------------------
/**
 * @file HAL_MCU11_revA.h
 * @author MIE
 * @brief 
 * Einlsen von Encoder
 * Setzten von LD 1-3
 * Setzen von P_OEN
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

//#define DEBUGGING_MCU11_revA    //P_OEN und Buzzer pin wird verwedet für debugging
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
    
    void begin(void (*INT_callBack)(void));
    void mapEncoder     (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);
    void mapBuzzer      (Output* P_BUZZER_OBJECT);
    void mapLD1         (Output* P_LD1_OBJECT);
    void mapLD2         (Output* P_LD2_OBJECT);
    void mapLD3         (Output* P_LD3_OBJECT);
    void mapOEN         (Output* P_OEN_OBJECT);

    void tick();

    DigitalInput*    P_Encoder_A;
    DigitalInput*    P_Encoder_B;
    DigitalInput*    P_Encoder_Z;

    Output*          P_BUZZER;
    Output*          P_LD1;
    Output*          P_LD2;
    Output*          P_LD3;   
    Output*          P_OEN;             

    private:
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
        const uint8_t encoder[3]= {39, 36, 34};
        const uint8_t led[3]    = {27, 26, 25};
        #ifdef DEBUGGING_MCU11_revA
        const uint8_t P_OEN       = 26;
        #endif
        #ifndef DEBUGGING_MCU11_revA
        const uint8_t P_OEN       = 13;
        #endif
        const uint8_t INT       = 35;
        #ifdef DEBUGGING_MCU11_revA
        const uint8_t buzzer       = 26;
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
    
    void begin(void (*INT_callBack)(void));
    void mapEncoder     (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);
    void mapBuzzer      (Output* P_BUZZER_OBJECT);
    void mapLD1         (Output* P_LD1_OBJECT);
    void mapLD2         (Output* P_LD2_OBJECT);
    void mapLD3         (Output* P_LD3_OBJECT);
    void mapOEN         (Output* P_OEN_OBJECT);

    void tick();
   
    private:

    DigitalInput*    P_Encoder_A;
    DigitalInput*    P_Encoder_B;
    DigitalInput*    P_Encoder_Z;

    Output*          P_BUZZER;
    Output*          P_LD1;
    Output*          P_LD2;
    Output*          P_LD3;   
    Output*          P_OEN;  
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
        const uint8_t encoder[3]= {39, 36, 34};
        const uint8_t led[3]    = {27, 26, 25};
        const uint8_t P_OEN       = 2;   
        const uint8_t INT       = 35;
        const uint8_t buzzer    = 5;       
    }PIN;    
};
#endif