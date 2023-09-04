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
#include "OLED_DISPLAY.h" 
#include "IOM_base.h"

typedef enum
{
    APP_MODE__STOP,    
    APP_MODE__START,
    APP_MODE__SAFE_STATE,
    APP_MODE__RUN_WITH_CONFIG_1,
    APP_MODE__RUN_WITH_CONFIG_2,
    APP_MODE__RUN_WITH_CONFIG_3,

    APP_MODE__COUNT,
}e_APP_MODE_t;

class APP_MCU11
{
    public:
    APP_MCU11();
    void begin();
    void tick();

    e_APP_MODE_t    getDeviceMode();    
    void            setDeviceMode(const e_APP_MODE_t MODE);

    //Dip Controll
    void    setVDip(const uint8_t DIP_NUM, const uint8_t VALUE);
    int     getVDip(const uint8_t DIP_NUM);
    
    private:
    HAL_MCU11   hal;
    OLED_MCU11  oled;

    e_APP_MODE_t    deviceMode;

    //Variablen
    int         virtualDipSwitch[8];
    uint8_t     errorCode[8];
    
    int temp_ParameterStorage;      //Temporärer Speicher für Parameter der gerade über das Oled bearbeitet wird

    void handleDisplay();
    void beepOnEncoderInput();

    void editDeviceMode();
    void errorOut();

    struct 
    {
        bool f_beepOnEncoderInput;
    }deviceSettings;
    
};
#endif