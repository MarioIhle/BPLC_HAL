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

//#define DEBUG_APP_MCU11_OLED_HANDLING

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

typedef enum
{
    //MCU Errors
    APP_ERROR__NO_ERROR,
    APP_ERROR__WTD_TIMEOUT,
    //I2C Kommunikation unterbrochen
    APP_ERROR__OLED_COMMUNICATION_FAILED = 10,
    APP_ERROR__DIN11_COMMUNICATION_FAILED,
    APP_ERROR__AIN11_COMMUNICATION_FAILED,
    APP_ERROR__REL11_COMMUNICATION_FAILED,
    APP_ERROR__DO11_COMMUNICATION_FAILED,
    APP_ERROR__MOT11_COMMUNICATION_FAILED,
    APP_ERROR__FUSE11_COMMUNICATION_FAILED,
    APP_ERROR__NANO_11_COMMUNICATION_FAILED,
    //MOT11 Errrors
    APP_ERROR__EXTERNAL_APP_ERROR = 30,
    APP_ERROR__MOT11_OVER_CURRENT,
    APP_ERROR__MOT11_OVER_TEMP,    
    //Error aus Applikation 
    APP_ERROR__EXTERNAL = 40,  
}e_APP_ERROR_t;


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
    void    setError(const e_APP_ERROR_t ERROR_CODE);
    void    resetError();
    
  
    private:
    HAL_MCU11   hal;
    OLED_MCU11  oled;

    e_APP_MODE_t    deviceMode;

    //Variablen
    uint8_t         virtualDipSwitch[vDIP_COUNT];
    e_APP_ERROR_t   errorCode;          //Sobald ein Error anliegt, wird Applikation gestoppt und im Safemode auf Userinput gewartet
    
    byte temp_ParameterStorage;         //Temporärer Speicher für Parameter der gerade über das Oled bearbeitet wird

    e_APP_ERROR_t checkForErrors();     //return=0, wenn kein Error gesetzt

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