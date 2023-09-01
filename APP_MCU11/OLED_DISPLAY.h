#ifndef OLED_DISPLAY_h
#define OLED_DISPLAY_h
//---------------------------------------------------
/**
 * @file OnBoardPavis_h.h
 * @author MIE
 * @brief 
 * Steuerbar mithilfe des Encoders und Oled Display
 * Fehlerausgabe
 * Onboard IOs setztbar/lesbar
 * 
 * @version 3.0
 * @date 2022-12-19 * 
 */
//---------------------------------------------------

#include "Arduino.h"

//Lib includes
#include "IOM_base.h"
#include "SpecialFunctions.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


//---------------------------------------------------
//DISPLAY PARAMETER
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      4
//---------------------------------------------------
//MENÜS
typedef enum
{        
    menu_deviceMode,
    menu_errorCodes,
    menu_settings,
    menu_dipSwitch,
    subMenu_count,

    menu_mainMenu,
}e_oledMenu_t;

typedef struct
{
  e_oledMenu_t      activeMenu;
  e_oledMenu_t      previousActiveMenu;

  int               activeText;             
}s_menuParameter_t;
//---------------------------------------------------
//MODES
typedef enum
{   
    mode_screenSaver,
    mode_unlocked,
    mode_count,
}e_oledMode_t;
//---------------------------------------------------
//DISPLAY 
typedef struct
{     
    int             cursorPos;      
    e_oledMode_t    mode;
    e_oledMode_t    previousMode;            
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
    Timeout         to_sleep;
    e_oledMode_t    modeBeforeScreenSaver; 
}s_screenSaverParameter_t;


//---------------------------------------------------
//MCU11 KLASSE
//---------------------------------------------------
class OLED_MCU11
{
    public:
            OLED_MCU11  ();
    void    begin       ();
    void    tick        ();

    //Steuerung 
    void        accept();   
    void        cursorUp();
    void        cursorDown();

    //Informationsaustasch
    void        setDeviceModeToShow(const uint8_t MODE);

    private:
    uint8_t     getMenuText     (const uint8_t LAST_TEXT, const int PREVIOUS_TEXT);
    void        showMenuText    (const String TEXT);
    
    void        screenSaver     ();
    void        showmainMenu    ();
    void        showDeviceMode  ();
    void        showErrorCodes  ();
    void        showSettings    ();
    void        showDipswitches ();

    bool                        digitalDipSwitch[8];

    bool                        f_accept;
    bool                        f_encoderUp;
    bool                        f_encoderDown;
    uint8_t                     deviceMode;

    s_display_t                 display;
    s_menuParameter_t           menu;
    s_screenSaverParameter_t    screenSaverParameter;
    s_deviceSettingsParameter_t deviceSettings;        
};

#endif