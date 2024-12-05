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
#include "BPLC_errorCodes.h"

#include "SpecialFunctions.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "I2C_check.h"
#include "BPLC_errorHandler.h"

//---------------------------------------------------
//DISPLAY PARAMETER
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      4

#define ROW_1 0
#define ROW_2 1
#define ROW_COUNT 2
//---------------------------------------------------
//MENÜS
typedef enum
{        
    OLED_STATE__SHOW_PAGE,
    OLED_STATE__ERROR,
    
}e_OLED_STATE_t;

typedef struct 
{
    struct 
    {
        String   text;
        bool     f_blink;
    }line[2];
    
}s_oledStandartMenuPage_t;

//---------------------------------------------------
//OLED KLASSE
//---------------------------------------------------
class OLED_STANDART_MENU:I2C_check, private BPLC_moduleErrorHandler, public BPLC_moduleErrorInterface
{
    public:
            OLED_STANDART_MENU  ();
    void    begin               ();
    void    tick                ();        
    void    setPage             (const s_oledStandartMenuPage_t PAGE); 
    
    //Modulerror Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

    
    private:
    e_OLED_STATE_t state;
    
    void        showPage        ();
    void        showText        (const String TEXT, const uint8_t ROW);
       
    //Textausgabe
    bool                        f_refreshPage;
    uint8_t                     cursorPos;
    s_oledStandartMenuPage_t    menuPage;    
    bool                        f_blinkState;
    blink                       blinkState;

    Adafruit_SSD1306            oled;
};

#endif