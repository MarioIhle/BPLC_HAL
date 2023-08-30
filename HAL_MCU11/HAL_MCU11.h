#ifndef HAL_DO11_h
#define HAL_DO11_h
//---------------------------------------------------
/**
 * @file OnBoardPavis_h.h
 * @author MIE
 * @brief 
 * Einfache Pavis funktionen steuerbar mithilfe des Encoders und Oled Display
 * Fehlerausgabe
 * Modus Diagnose parametrierbar
 * Türbefehle (EFX/DFX/usw...)
 * Autotuning
 * Grundstellung
 * Onboard IOs setztbar/lesbar
 * 
 * @version 3.0
 * @date 2022-12-19 * 
 */
//---------------------------------------------------

//#include "Arduino.h"







class MCU11
{
    MCU11();
    void begin();

    public:

    private:

    

};



//Lib includes
#include "SpecialFunctions.h"
#include "Datalink_V3.h"
#include "ioManager_work.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//---------------------------------------------------
//DISPLAY PARAMETER
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      4
//---------------------------------------------------
//MENÜS
typedef enum
{        
    menu_errorCodes,
    menu_diagnose,
    menu_dipSwitches,
    menu_teachIn,
    menu_deviveSettings,
    subMenu_count,
    menu_mainMenu,
}e_OnBoardPavisMenu_t;

typedef struct
{
  e_OnBoardPavisMenu_t    active;
  e_OnBoardPavisMenu_t    previousActive;
  uint8_t                 activeText;
}s_menuParameter_t;
//---------------------------------------------------
//MODES
typedef enum
{   
    mode_screenSaver,
    mode_unlocked,
    mode_count,
}e_OnBoardPavisMode_t;
//---------------------------------------------------
//DISPLAY 
typedef struct
{     
    int16_t                 cursorPos;    
    e_OnBoardPavisMode_t    mode;
    e_OnBoardPavisMode_t    previousMode;            
}s_display_t;
//---------------------------------------------------
//DEVICE SETTINGS
#define UNLOCK_CODE_LENGTH  	4       //Länge des Sperrbildschrim codes

typedef struct{
    uint32_t    sleepTime;
    uint32_t    lockTime;
    bool        screenSaverIsEnbaled;
}s_deviceSettingsParameter_t;
//---------------------------------------------------
//SCREENSAVER
typedef struct{          
    Timeout                 to_sleep;
    e_OnBoardPavisMode_t    modeBeforeScreenSaver; 
}s_screenSaverParameter_t;

typedef struct
{   
    RotaryEncoder*  p_knob;
    DigitalOutput*  p_LD1;
    DigitalOutput*  p_LD2;  
    DigitalOutput*  p_LD3;     
}s_IO_t;

//---------------------------------------------------
//ONBOARD PAVIS KLASSE
//---------------------------------------------------
class OnBoardPavis
{
    public:
    OnBoardPavis();
    void begin  (s_OnBoardPavisParameter_t INIT);
    void tick();

    private:
    //Display Ausgabe
    void        mainMenu        ();
    void        doorCommandsMenu();
    uint8_t     getMenuText     (const uint8_t LAST_TEXT, const uint8_t PREVIOUS_TEXT);
    void        showMenuText    (const String TEXT);
    void        screenSaver     ();
    void        showErrorCodes  ();
    uint32_t    useKnobAsInput  (uint32_t VALUE);

    s_display_t                 display;
    s_menuParameter_t           menu;
    s_screenSaverParameter_t    screenSaverParameter;
    s_deviceSettingsParameter_t deviceSettings;
    s_IO_t                      IOs;
};


#endif