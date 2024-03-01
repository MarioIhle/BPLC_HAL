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
#include "BPLC_ioBaseTypes.h"
#include "BPLC_PLI.h"
#include "HAL_interface.h"


//#define DEBUGGING_MCU11_revA    //p_oen und Buzzer pin wird verwedet für debugging

enum
{
    MCU_CHANNEL__ENCODER,
    MCU_CHANNEL__BUZZER,
    MCU_CHANNEL__OEN,
    MCU_CHANNEL__INT,
    MCU_CHANNEL__LD1,
    MCU_CHANNEL__LD2,
    MCU_CHANNEL__LD3,

    MCU_CHANNEL__COUNT,
};
//--------------------------------------------------------------------
//HAL KLASSE
//--------------------------------------------------------------------
class HAL_MCU11_revA: public halInterface, BPLC_errorHandler, BPLC_LOG
{
    public:
                    HAL_MCU11_revA      (volatile uint64_t* P_ISR_COUNT);
    void            init                ();
    void            tick                ();
    e_BPLC_ERROR_t  getErrorCode        (){return this->getError();}
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);
   

    private:   
    void            tickSafety          ();
    IO_Interface*   p_encoder;
    IO_Interface*   p_buzzer;
    IO_Interface*   p_ld1;
    IO_Interface*   p_ld2;
    IO_Interface*   p_ld3;   
    IO_Interface*   p_oen;             

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
        const uint8_t ENCODER_A = 39;
        const uint8_t ENCODER_B = 36;
        const uint8_t ENCODER_BUTTON = 34;  
        const uint8_t LD1       = 27;
        const uint8_t LD2       = 26;
        const uint8_t LD3       = 25;
        #ifdef DEBUGGING_MCU11_revA
        const uint8_t OEN       = 25;
        #endif
        #ifndef DEBUGGING_MCU11_revA
        const uint8_t OEN       = 13;
        #endif
        const uint8_t INT       = 35;
        #ifdef DEBUGGING_MCU11_revA
        const uint8_t BUZZER    = 25;
        #endif
        #ifndef DEBUGGING_MCU11_revA
        const uint8_t BUZZER    = 15;
        #endif
    }PIN;   
};

class HAL_MCU11_revB: public halInterface, BPLC_errorHandler, BPLC_LOG
{
    public:
                    HAL_MCU11_revB      (volatile uint64_t* P_ISR_COUNT);    
    void            init                ();
    void            tick                ();
    e_BPLC_ERROR_t  getErrorCode        (){return this->getError();}
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);

    private:
    void            tickSafety();

    IO_Interface* p_encoder;
    IO_Interface* p_buzzer;
    IO_Interface* p_ld1;
    IO_Interface* p_ld2;
    IO_Interface* p_ld3;   
    IO_Interface* p_oen;  
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
        const uint8_t ENCODER_A         = 39;
        const uint8_t ENCODER_B         = 36;
        const uint8_t ENCODER_BUTTON    = 34;  
        const uint8_t LD1               = 27;
        const uint8_t LD2               = 26;
        const uint8_t LD3               = 25;
        const uint8_t OEN               = 2;   
        const uint8_t INT               = 35;
        const uint8_t BUZZER            = 5;       
    }PIN;    
};
#endif