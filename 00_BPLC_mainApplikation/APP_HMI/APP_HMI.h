#ifndef APP_HMI_h
#define APP_HMI_h

#include "Arduino.h"
#include "BPLC_types.h"
#include "OLED_DISPLAY.h"
#include "03_BPLC_ioBaseTypes/BPLC_ioBaseTypes.h"

typedef enum
{
    HMI_MENU__MAIN_MENU,
    HMI_MENU__EDIT_DEVICE_MODE,
    HMI_MENU__EDIT_V_DIP,
    HMI_MENU__EDIT_SETTING,

    HMI_MENU__COUNT,
}e_HMI_MENU_t;


class BPLC_HMI
{          
    public:
            BPLC_HMI    ();
    void    begin       (hmiEncoder* P_HMI_ENCODER, output* P_BUZZER);
    void    tick        ();


    private:    
    void showMainMenu       (const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION);
    void editDeviceMode     (const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION);
    void hardwareErrorOut   (const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION);
    void displaySettings    (const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION);
    void handle_vDip        (const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION);

    struct 
    {
        e_BPLC_DEVICE_MODE_t deviceMode;
    }data;

    struct 
    {
        e_HMI_MENU_t    activeMenu;
        uint8_t         activeHeadlineTextOfMenu;
    }menu;
    
    
    struct 
    {
        bool f_beepOnEncoderInput;
    }settings; 

    hmiEncoder*         p_mcuHmiEncoder;
    output*             p_buzzer;
    OLED_STANDART_MENU  oledDisplay;   
};

#endif