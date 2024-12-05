#ifndef APP_HMI_h
#define APP_HMI_h

#include "Arduino.h"
#include "BPLC_types.h"
#include "OLED_DISPLAY.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_errorHandler.h"

typedef enum
{
    HMI_MENU__SCREENSAVER,
    HMI_MENU__MAIN_MENU,
    HMI_MENU__EDIT_DEVICE_MODE,
    HMI_MENU__ERROR_OUT,
    HMI_MENU__EDIT_V_DIP,
    HMI_MENU__EDIT_SETTING,
    
    HMI_MENU__COUNT,
    
}e_HMI_MENU_t;

#define MENU_DEF_TEXT       0
#define MENU_DEF_PAGE_COUNT 1
#define MENU_DEF_HAS_PARAM  2 

typedef enum
{   
   EDITABLE_PARAM__NO,
   EDITABLE_PARAM__BOOL,
   EDITABLE_PARAM__U8,

}e_EDITABLE_PARAM_t;

typedef struct 
{   
   String*              p_texts[ROW_COUNT];              
   uint8_t              menuPageCount;
   e_EDITABLE_PARAM_t   paramType;
   e_HMI_MENU_t         nextMenu;

}s_menu_t;

class BPLC_HMI: private BPLC_moduleErrorHandler, public BPLC_moduleErrorInterface
{          
    public:
            BPLC_HMI    ();
    void    begin       (hmiEncoder* P_HMI_ENCODER, output* P_BUZZER);
    void    tick        ();

    //Modulerror Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

    private:    

    void showMenu           (uint8_t* p_tempParamValue);

    struct 
    {
        e_BPLC_DEVICE_MODE_t deviceMode;
    }data;

    struct 
    {
        e_HMI_MENU_t    activeMenu;
        uint8_t         activeText;
        bool            f_editShownParameter;
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