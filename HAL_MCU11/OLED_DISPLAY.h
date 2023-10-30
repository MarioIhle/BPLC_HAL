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
#include "SpecialFunctions.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "BPLC_ERRORS.h"


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
    OLED_MENU__MAIN,
    OLED_MENU__DEVICE_MODE,
    OLED_MENU__BPLC_HARDWARE,
    OLED_MENU__VDIP,
    OLED_MENU__DEVICE_SETTINGS,

    OLED_MENU__SCREENSAVER,

    OLED_MENU__COUNT,
}e_OLED_MENU_t;

typedef struct 
{
    e_OLED_MENU_t   name;
    uint8_t         textCount;
    String          texts[30];   

}s_menu_t;


//---------------------------------------------------
//OLED MCU11 KLASSE
//---------------------------------------------------
class OLED_MCU11
{
    public:
            OLED_MCU11  ();
    void    begin       (s_menu_t* P_MENUS, const uint8_t MENU_COUNT);
    void    tick        ();

    void showScreenSaver ();

    bool readyToExitMenu ();
    void setTextToShow   (const String TEXT, const bool ROW);
    void blinkText       (const bool ROW, const unsigned long INTERVAL);



    //Menüsteuernug
    //void            showNextTextOfThisMenu      ();
    //void            showPrevioursTextOfThisMenu ();
    //e_OLED_MENU_t   getActiveMenu               ();      
    //uint8_t         getActiveTextOfThisMenu     ();
    //void            setParamValueToShow         (const uint8_t VALUE);    

    private:
    Adafruit_SSD1306 oled;

    void showMenuText    (const String TEXT, const bool ROW);  
    
           
    struct 
    {
        bool            f_refresh;
        uint8_t         paramValue;

        bool            f_blinkRow_0;
        bool            f_blinkRow_1;
        Timeout         to_textBlink;
        bool            f_parameterBlink;
    
        String          TEXT_OUTPUT[2];
    }display;
     

    ERROR_OUT errorOut;

    struct
    {          
        Timeout         to_sleep;
    }screenSaverParameter;  

    struct
    {
    uint32_t    sleepTime;
    bool        screenSaverIsEnbaled;
    }deviceSettings;      
};
#endif