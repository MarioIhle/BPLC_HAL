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
#include "BPLC_IOM.h"
#include "BPLC_LOG.h"
#include "SpecialFunctions.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "BPLC_ERRORS.h"
#include "I2C_check.h"

//#define DEBUG_OLED_DISPLAY


//---------------------------------------------------
//DISPLAY PARAMETER
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      4
//---------------------------------------------------
//MENÜS
typedef enum
{        
    menu_mainMenu,
    menu_deviceMode,
    menu_errorCodes,
    menu_dipSwitch,
    menu_settings,

    menu_screenSaver,

    menu_count,
}e_oledMenu_t;

typedef struct
{
  e_oledMenu_t      activeMenu;
  e_oledMenu_t      previousActiveMenu;

  int               activeText;             
}s_menu_t;

typedef struct
{     
    int             cursorPos;    
}s_display_t;
//---------------------------------------------------
//DEVICE SETTINGS
typedef struct{
    uint32_t    sleepTime;
    bool        screenSaverIsEnbaled;
}s_deviceSettingsParameter_t;
//---------------------------------------------------
//SCREENSAVER
typedef struct{          
    Timeout         to_sleep;
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

    //Menüsteuernug
    void            showNextTextOfThisMenu      ();
    void            showPrevioursTextOfThisMenu ();
    void            enterMenu                   ();
    e_oledMenu_t    getActiveMenu               ();      
    uint8_t         getActiveMenuTextNum        ();
    void            setMenu                     (const e_oledMenu_t MENU);
    bool            readyToExitMenu             ();
    //Parametereingabe
    void            enterParameter              ();
    void            exitParameter               ();
    bool            parameterEntered            ();
    void            setParamValueToShow         (const uint8_t VALUE);    
    e_BPLC_ERROR_t  getError                    ();

    private:
    Adafruit_SSD1306 oled;

    void        showHeadlineText();       
    void        showMenuText    (const String TEXT, const bool ROW);
    uint8_t     getMenuText     (const uint8_t LAST_AVAILABLE_TEXT, const uint8_t ACTIVE_TEXT);
  
    void        showScreenSaver      ();
    void        showMainMenu         ();
    void        showDeviceMode       ();
    void        showHardwareErrorCode();
    void        showSettings         ();
    void        showDipswitches      ();    

    s_display_t display;
    s_menu_t    menu;
    bool        f_refresh;

    int16_t         paramValue;
    bool            f_parmParameter;
    Timeout         to_parmeter;
    bool            f_parameterBlink;
 
    String          TEXT_OUTPUT[2];

    ERROR_OUT errorOut;

    s_screenSaverParameter_t    screenSaverParameter;
    s_deviceSettingsParameter_t deviceSettings;      

    e_BPLC_ERROR_t  errorCode; 
    I2C_check       selfCheck;
};

#endif