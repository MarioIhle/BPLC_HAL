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
    menu_errorCodes,
    menu_diagnose,
    menu_dipSwitches,
    menu_teachIn,
    menu_deviveSettings,
    subMenu_count,
    menu_mainMenu,
}e_oledMenu_t;

typedef struct
{
  e_oledMenu_t    active;
  e_oledMenu_t    previousActive;
  uint8_t         activeText;
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
    int16_t         cursorPos;    
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

//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
typedef enum
{
    idle,
    left,
    right,
}e_direction_t;

//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class DigitalInput
{
    public:
    DigitalInput();
    DigitalInput(const uint8_t PIN);

    void    begin       (const uint8_t PIN);
    bool 	ishigh		();
    bool	islow		();
	bool 	posFlank	();	
	bool 	negFlank	();	

    private:
    uint8_t    pin;
    bool        lasteState;
};

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder {
	public:
    RotaryEncoder(const uint8_t A, const uint8_t B, const uint8_t Z);
   
    void                begin   (const uint8_t A, const uint8_t B, const uint8_t Z);

    e_direction_t       getTurningDirection();
    bool                buttonPressed();
    
  	private:
    DigitalInput   A;
    DigitalInput   B; 
    DigitalInput   Z; 
};


//---------------------------------------------------
//MCU11 KLASSE
//---------------------------------------------------
class MCU11
{
    public:
    MCU11();
    void begin();
    void tick();

    private:
    //Display Ausgabe
    void        mainMenu        ();
    uint8_t     getMenuText     (const uint8_t LAST_TEXT, const uint8_t PREVIOUS_TEXT);
    void        showMenuText    (const String TEXT);
    void        screenSaver     ();
    void        showErrorCodes  ();
    uint32_t    useKnobAsInput  (uint32_t VALUE);

    s_display_t                 display;
    s_menuParameter_t           menu;
    s_screenSaverParameter_t    screenSaverParameter;
    s_deviceSettingsParameter_t deviceSettings;

    RotaryEncoder   knob;
    DigitalOutput*  p_LD1;
    DigitalOutput*  p_LD2;  
    DigitalOutput*  p_LD3;  

    struct 
    {
        struct 
        {
            const uint8_t A = 39;
            const uint8_t B = 36;
            const uint8_t Z = 34;
        }encoder;

        struct 
        {
            const uint8_t LD1 = 27;
            const uint8_t LD2 = 26;
            const uint8_t LD3 = 25;
        }led;
    }pins;
};

#endif