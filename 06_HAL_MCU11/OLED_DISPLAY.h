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
#include "BPLC_ioBaseTypes.h"
#include "BPLC_errorCodes.h"

#include "SpecialFunctions.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "I2C_check.h"

//---------------------------------------------------
//DISPLAY PARAMETER
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      4
//---------------------------------------------------
//MENÜS
typedef enum
{        
    OLED_STATE__SCREENSAVER,
    OLED_STATE__SHOW_PAGE,
    OLED_STATE__ERROR,
    
}e_OLED_STATE_t;

typedef struct 
{
    struct 
    {
        String text;
        bool f_blink;
    }line[2];
    
}s_oledStandartMenuPage_t;

//---------------------------------------------------
//OLED KLASSE
//---------------------------------------------------
class OLED_STANDART_MENU:I2C_check
{
    public:
            OLED_STANDART_MENU  ();
    void    begin               ();
    void    tick                ();        
    void    setPage             (const s_oledStandartMenuPage_t PAGE); 
    void    resetScreenSaver    ();
    
    e_BPLC_ERROR_t  getError    (){return this->errorCode;}

    
    private:
    e_OLED_STATE_t state;
    
    void        showPage        ();
    void        showText        (const String TEXT, const bool ROW);
       
    //Textausgabe
    bool                        f_refreshPage;
    uint8_t                     cursorPos;
    s_oledStandartMenuPage_t    menuPage;    
    bool                        f_blinkState;
    blink                       blinkState;
    Timeout                     to_sleep;    

    ERROR_errorText             errorOut;
    e_BPLC_ERROR_t              errorCode; 

    Adafruit_SSD1306            oled;
};

#endif