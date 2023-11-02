#ifndef BPLC_APP_h
#define BPLC_APP_H
//------------------------------------------------------------
/**
 * @file BPLC_APP.h
 * @author MIE
 * @brief 
 * Steuerbar mithilfe des Encoders und Oled Display
 * Fehlerausgabe
 * Onboard IOs setztbar/lesbar
 * 
 * @version 1.0
 * @date 2022-12-19 * 
 */
//-------------------------------------------------------------

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"

#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"

#include "HAL_MCU11.h"
#include "OLED_DISPLAY.h" 
#include "HAL_DIN11.h"
#include "HAL_AIN11.h"
#include "HAL_REL11.h"
#include "HAL_FUSE11.h"
#include "HAL_DO11.h"
#include "HAL_MOT11.h"

//-------------------------------------------------------------
//HARDWARE DEBUGGING
//-------------------------------------------------------------
//#define DEBUG_APP_MCU11_OLED_HANDLING

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
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

//-------------------------------------------------------------
//OLED SPEZIFISCHE TYPES
//-------------------------------------------------------------
//MENÜS
typedef enum
{        
    OLED_MENU__MAIN,
    OLED_MENU__DEVICE_MODE,
    OLED_MENU__VDIP,
    OLED_MENU__BPLC_HARDWARE_STATE,
    OLED_MENU__MOT11_AUTOTUNING,
    OLED_MENU__DEVICE_SETTINGS,

    OLED_MENU__SCREENSAVER,
    OLED_MENU__BACK_TO_MAIN,

    OLED_MENU__COUNT,
}e_OLED_MENU_t;

//-------------------------------------------------------------
//BPLC_APP KLASSE
//-------------------------------------------------------------
#define HARDWARE_ERROR_BUFFER_SIZE 10

class BPLC_APP
{
    public:
    BPLC_APP();
    void begin();
    void defineHardwareSetup(const uint8_t DIN11_CARD_COUNT, const uint8_t AIN11_CARD_COUNT, const uint8_t DO11_CARD_COUNT, const uint8_t REL11_CARD_COUNT, const uint8_t MOT11_CARD_COUNT, const uint8_t FUSE11_CARD_COUNT, const uint8_t NANO11_CARD_COUNT);    
    void mapObjectToCard(DigitalInput* P_OBJECT, const e_DIN11_CARD_t CARD);    //Je nach Reihenfolge werden Ports vergeben
    void mapObjectToCard(Output* P_OBJECT, const e_DO11_CARD_t CARD);
    void mapObjectToCard(AnalogInput* P_OBJECT, const e_AIN11_CARD_t CARD);
    void mapObjectToCard(Output* P_OBJECT, const e_REL11_CARD_t CARD);
    void mapObjectToCard(MOTOR* P_OBJECT, const e_MOT11_CARD_t CARD);
    void mapObjectToCardAndPort(DigitalInput* P_OBJECT, const e_DIN11_CARD_t CARD, const e_DIN11_PORTS_t PORT);    //Je nach Reihenfolge werden Ports vergeben
    void mapObjectToCardAndPort(Output* P_OBJECT,const e_DO11_CARD_t CARD, const e_DO11_PORTS_t);
    void mapObjectToCardAndPort(AnalogInput* P_OBJECT, const e_AIN11_CARD_t CARD, const e_AIN11_PORTS_t PORT);
    void mapObjectToCardAndPort(Output* P_OBJECT, const e_REL11_CARD_t CARD, const e_REL11_PORTS_t PORT);

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
    bool isThereAnyHardwareError();
    
    private:
    HAL_MCU11   hal;
    OLED_MCU11  oled;    

    e_APP_MODE_t    deviceMode;
    uint8_t         virtualDipSwitch[vDIP_COUNT];
    e_BPLC_ERROR_t  hardwareErrorCode[HARDWARE_ERROR_BUFFER_SIZE];   //Hardware Error, sofort Applikation anhalten. Es wird immer der letzte erkannte Fehler in der nächsten freien stelle gespeichert gespeichert
        
    //Applikation
    struct 
    {
        e_APP_MODE_t deviceModeOld;
    }debug;
    
    //Display handling
    void displayBegin();
    void handleDisplay();
    bool readyToExitMenu(const uint8_t SHOWN_TEXT, const uint8_t LAST_TEXT_OF_THIS_MENU);
    void useEncoderForNavigation(uint8_t* P_VALUE_TO_EDIT, const uint8_t MAX_VALUE);

    void beepOnEncoderInput();
    void showMainMenu();
    void editDeviceMode();
    void hardwareState();
    void mot11CurrentAutotuning();
    void displaySettings();
    void handle_vDip();

    void showAnyDefinedMenu(s_menu_t MENU, String VAR);

    s_menu_t MENU_DEFINITION[2];

    struct 
    {
        bool    f_editParameterOnDisplay;
        uint8_t tempValueToShow;
 
        e_OLED_MENU_t   activeMenu;
        e_OLED_MENU_t   previousActiveMenu;
        uint8_t         menuCount;
        uint8_t         activeText;            
   
    }displayHandling;    

    

    //Hardware Handling
    void setupExtensionCards();
    void ISR_CALLED();
    void handleDIN11Cards();
    void handleDO11Cards();
    void handleAIN11Cards();
    void handleMOT11Cards();
    void handleREL11Cards();

    //Hal objecte zu allen möglichen Erweiterungskarten
    HAL_DIN11 DIN11_CARD[DIN11_CARD__MAX]; 
    HAL_AIN11 AIN11_CARD[AIN11_CARD__MAX];
    HAL_DO11  DO11_CARD [DO11_CARD__MAX];
    HAL_REL11 REL11_CARD[REL11_CARD__MAX];
    HAL_MOT11 MOT11_CARD[MOT11_CARD__MAX];  //eigentlich unendlich erweiterbar, da Atm328p und software addresse
    struct
    {
        uint8_t din11CardCount;
        uint8_t ain11CardCount;
        uint8_t do11CardCount;
        uint8_t rel11CardCount;
        uint8_t mot11CardCount;
        uint8_t fuse11CardCount;
        uint8_t nano11CardCount;
    }hardware;
    
    //Settings
    struct 
    {
        bool f_beepOnEncoderInput;
        bool f_useBuzzer;
    }deviceSettings;    

    //Safety
    Timeout to_runnntime;
    ERROR_OUT errorOut;
};
#endif