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

#define DEBUG_APP_MCU11_OLED_HANDLING

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
    vDIP_1,
    vDIP_2,
    vDIP_3,
    vDIP_4,
    vDIP_5,
    vDIP_6,
    vDIP_7,
    vDIP_8,

    vDIP_COUNT,
}e_V_DIP_t;


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
    void begin(void (*INT_callBack)(void));
    void tick();

    e_APP_MODE_t    getDeviceMode();    
    void            setDeviceMode(const e_APP_MODE_t MODE);

    void    beep(const uint8_t BEEPS, const int BEEP_INTERVAL);

    //Dip Controll
    void    setVDip(const e_V_DIP_t DIP_NUM, const uint8_t VALUE);
    int     getVDip(const e_V_DIP_t DIP_NUM);

    //Error flags
    void    setError();
    
  
    private:
    HAL_MCU11   hal;
    OLED_MCU11  oled;

    e_APP_MODE_t    deviceMode;

    //Variablen
    uint8_t     virtualDipSwitch[vDIP_COUNT];
    uint8_t     errorCode[8];
    
    byte temp_ParameterStorage;      //Temporärer Speicher für Parameter der gerade über das Oled bearbeitet wird

    void handleDisplay();
    void beepOnEncoderInput();

    void editDeviceMode();
    void errorOut();
    void handle_vDip();

    struct 
    {
        bool f_beepOnEncoderInput;
    }deviceSettings;    
};
#endif