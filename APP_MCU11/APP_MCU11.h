#ifndef APP_MCU11_h
#define APP_MCU11_h
//---------------------------------------------------
/**
 * @file APP_MCU11.h
 * @author MIE
 * @brief 
 * Steuerbar mithilfe des Encoders und Oled Display
 * Fehlerausgabe
 * Onboard IOs setztbar/lesbar
 * 
 * @version 1.0
 * @date 2022-12-19 * 
 */
//---------------------------------------------------

#include "Arduino.h"
//Lib includes
#include "SpecialFunctions.h"

//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
#include "HAL_MCU11.h"
//#include "OLED_DISPLAY.h" 

typedef enum
{
    APP_MODE__STOP,
    APP_MODE__START,
    APP_MODE__RUNNING,
    APP_MODE__SAFE_STATE,

    APP_MODE__COUNT,
}e_APP_MODE_t;

class APP_MCU11
{
    public:
    APP_MCU11();
    void begin();
    void tick();

    private:
    HAL_MCU11   hal;
    //OLED_MCU11  oled;
    

    e_APP_MODE_t    MODE;

};
#endif