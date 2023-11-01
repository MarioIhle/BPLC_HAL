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
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "SpecialFunctions.h"

//#define DEBUG_OLED_DISPLAY


//---------------------------------------------------
//DISPLAY PARAMETER
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      4
//---------------------------------------------------


typedef struct 
{
    uint8_t name;
    uint8_t nameOfMenuWhenExiting;
    uint8_t textCount;
    String  texts[30];   

}s_menu_t;

typedef enum
{
    OLED_ROW__1,
    OLED_ROW__2,
    OLED_ROW__3,
    OLED_ROW__4,

    OLED_ROW__COUNT,
}e_OLED_ROW_t;

//---------------------------------------------------
//OLED MCU11 KLASSE
//---------------------------------------------------
class OLED_MCU11
{
    public:
    OLED_MCU11  ();
    void begin  ();
    void tick   ();    

    void setTextToShow (const String TEXT, const e_OLED_ROW_t ROW);
    void clearAllTexts();
    void startBlinkText(const e_OLED_ROW_t ROW, const unsigned long INTERVAL);
    void stopBlinkText (const e_OLED_ROW_t ROW);

    //Menüsteuernug
    //void            showNextTextOfThisMenu      ();
    //void            showPrevioursTextOfThisMenu ();
    //e_OLED_MENU_t   getActiveMenu               ();      
    //uint8_t         getActiveTextOfThisMenu     ();
    //void            setParamValueToShow         (const uint8_t VALUE);    

    private:
    Adafruit_SSD1306 oled;

    void            showText();
    bool            f_refresh;
    uint8_t         paramValue;

    bool            f_blinkRow[OLED_ROW__COUNT];

    Timeout         to_textBlink;
    bool            f_parameterBlink;

    String          TEXT_OUTPUT[OLED_ROW__COUNT];

};
#endif