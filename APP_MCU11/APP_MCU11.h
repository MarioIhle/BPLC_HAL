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
#include "SpecialFunctions.h"
#include "HAL_MCU11.h"
#include "OLED_DISPLAY.h" 
#include "IOM_base.h"
//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------

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
    BPLC_ERROR__NO_ERROR,
    //MCU Errors
    BPLC_ERROR__RUNNTIME,
    //I2C Kommunikation unterbrochen
    BPLC_ERROR__OLED_COMMUNICATION_FAILED ,
    BPLC_ERROR__DIN11_COMMUNICATION_FAILED,
    BPLC_ERROR__AIN11_COMMUNICATION_FAILED,
    BPLC_ERROR__REL11_COMMUNICATION_FAILED,
    BPLC_ERROR__DO11_COMMUNICATION_FAILED,
    BPLC_ERROR__MOT11_COMMUNICATION_FAILED,
    BPLC_ERROR__FUSE11_COMMUNICATION_FAILED,
    BPLC_ERROR__NANO_11_COMMUNICATION_FAILED,
    //MOT11 Errrors
    BPLC_ERROR__MOT11_OVER_CURRENT,
    BPLC_ERROR__MOT11_OVER_TEMPERATURE,   
    BPLC_ERROR__MOT11_CURRENT_NOT_TEACHED, 

    BPLC_ERROR__COUNT,
}e_BPLC_ERROR_t;


class APP_MCU11
{
    public:
    APP_MCU11();
    void begin(void (*INT_callBack)(void));
    void tick();
    
    e_APP_MODE_t    getDeviceMode();    
    void            setDeviceMode(const e_APP_MODE_t MODE);

    //Buzzer aus Applikation heraus nutzen
    void    beep(const uint8_t BEEPS, const int BEEP_INTERVAL);

    //Dip Controll
    void    setVDip(const e_V_DIP_t DIP_NUM, const uint8_t VALUE);
    int     getVDip(const e_V_DIP_t DIP_NUM);

    //Externer aufruf, wenn HAL Objekt ein Error meldet
    void setHardwareError(const e_BPLC_ERROR_t ERROR_CODE);
    
    private:
    HAL_MCU11   hal;
    OLED_MCU11  oled;

    e_APP_MODE_t    deviceMode;

    //Variablen
    uint8_t         virtualDipSwitch[vDIP_COUNT];

    byte temp_ParameterStorage;         //Temporärer Speicher für Parameter der gerade über das Oled bearbeitet wird

    bool isThereAnyHardwareError();     //return=0, wenn kein Error gesetzt
    e_BPLC_ERROR_t  hardwareErrorCode;          //Hardware Error, sofort Applikation anhalten
    
    void handleDisplay();
    void beepOnEncoderInput();

    void editDeviceMode();
    void errorOut();
    void handle_vDip();

    struct 
    {
        bool f_beepOnEncoderInput;
    }deviceSettings;    

    //Safety
    Timeout to_runnntime;
};
#endif