#include "APP_HMI.h"

BPLC_HMI::BPLC_HMI()
{}

void BPLC_HMI::begin(hmiEncoder* P_HMI_ENCODER, output* P_BUZZER)
{
   this->p_mcuHmiEncoder            = P_HMI_ENCODER;
   this->p_buzzer                   = P_BUZZER;        
   this->menu.activeMenu            = HMI_MENU__MAIN_MENU;
   this->menu.f_editShownParameter  = false;

   this->oledDisplay.begin(); 
}
void BPLC_HMI::tick()
{  
   //Display ticken
   this->oledDisplay.tick();

   //Buzzer beep
   if(this->p_mcuHmiEncoder->buttonReleased())
   {
      this->p_buzzer->blinkOnce(1,50);
   }

   uint8_t tempValue = 88;

   //Menüsteuerung
   if(this->oledDisplay.getModuleErrorCode(0) == BPLC_ERROR__NO_ERROR)
   {
      switch(this->menu.activeMenu)
      {      
         case HMI_MENU__MAIN_MENU:
            this->showMenu(&tempValue);        
            break;

         case HMI_MENU__EDIT_DEVICE_MODE:
            this->showMenu(&tempValue); 
            break;

         case HMI_MENU__ERROR_OUT:
            this->showMenu(&tempValue); 
            break;

         case HMI_MENU__EDIT_SETTING:
            this->showMenu(&tempValue); 
            break;

         case HMI_MENU__EDIT_V_DIP:
            this->showMenu(&tempValue);         
            break;
      }
   }   
}


//Display handling
static String LOCK = "LOCK";
static String EXIT = "EXIT";
static String PRESS = "press";
static String SHOW_PARAM_INSTEAD = "param";

String screenSaver[2][1] =   
{  
   {{""}},
   {{""}}
};
String MainMenuTexts[2][HMI_MENU__COUNT] =   
{  
   {{"DEV MODE"},             {"EDIT MODE"}, {"ERROR"},  {"VDIP"},   {"SETTINGS"}},
   {SHOW_PARAM_INSTEAD,        PRESS,         PRESS,      PRESS,      PRESS}
};
String editDeviceModeTexts[2][BPLC_DEVICE_MODE__COUNT] =   
{  
   {{"EDIT MODE"}},
   {{"stop"},{"start"},{"safe"},{"run"},{"run no error"},{"run no hal"},{"run no com"}}
};
String errorOutTexts[2][1] =   
{  
   {{"ERROR CODE"}},   
   {SHOW_PARAM_INSTEAD}
};
String vDipTexts[2][8] =   
{  
   {{"vDip1"}, {"vDip2"}, {"vDip3"}, {"vDip4"}, {"vDip5"}, {"vDip6"}, {"vDip7"}, {"vDip8"}},
   {SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD}
};
String settingTexts[2][2] =   
{  
   {{"setting1"}, {"setting2"}}, 
   {SHOW_PARAM_INSTEAD, SHOW_PARAM_INSTEAD}
};

s_menu_t menuDefinition[HMI_MENU__COUNT] =  
{
   //{p_texts[ROW_1],                        p_texts[ROW_2]},                    menuPages,      f_hasParam,                      nextMenu}
   {{&screenSaver[ROW_1][0],                 &screenSaver[ROW_2][0]},            1,              EDITABLE_PARAM__NO,           HMI_MENU__MAIN_MENU},
   {{&MainMenuTexts[ROW_1][0],               &MainMenuTexts[ROW_2][0]},          5,              EDITABLE_PARAM__NO,           HMI_MENU__SCREENSAVER},
   {{&editDeviceModeTexts[ROW_1][0],         &editDeviceModeTexts[ROW_2][0]},    7,              EDITABLE_PARAM__U8,           HMI_MENU__MAIN_MENU},
   {{&errorOutTexts[ROW_1][0],               &errorOutTexts[ROW_2][0]},          1,              EDITABLE_PARAM__BOOL,         HMI_MENU__MAIN_MENU},
   {{&vDipTexts[ROW_1][0],                   &vDipTexts[ROW_2][0]},              8,              EDITABLE_PARAM__U8,           HMI_MENU__MAIN_MENU},
   {{&settingTexts[ROW_1][0],                &settingTexts[ROW_2][0]},           2,              EDITABLE_PARAM__NO,           HMI_MENU__MAIN_MENU},
};
             
void BPLC_HMI::showMenu(uint8_t* p_tempParamValue)
{   
   const e_HMI_MENU_t   ACTIVE_MENU                = this->menu.activeMenu;
   const uint8_t        ACTIVE_TEXT                = this->menu.activeText;   

   const s_menu_t*      P_ACTIVE_MENU_DEFINITION   = &menuDefinition[ACTIVE_MENU];
   const uint8_t        TEXT_COUNT_OF_ACTIVE_MENU  = P_ACTIVE_MENU_DEFINITION->menuPageCount;
   const bool           END_OF_MENU_REACHED        = (ACTIVE_TEXT > TEXT_COUNT_OF_ACTIVE_MENU);
   const bool           EDIT_PARAMETER             = ((P_ACTIVE_MENU_DEFINITION->paramType!= EDITABLE_PARAM__NO) && this->menu.f_editShownParameter);

   //Encoder
   const bool           ENCODER_BUTTON_PRESSED  = this->p_mcuHmiEncoder->buttonPressed();
   const e_MOVEMENT_t   ENCODER_DIRETION        = this->p_mcuHmiEncoder->getTurningDirection();   
   const bool           BEEP_ON_ENCODER_INPUT   = this->settings.f_beepOnEncoderInput;

   //Menüsteuerung
   if(ENCODER_BUTTON_PRESSED)
   {    
      //Menü verlassen
      if(END_OF_MENU_REACHED)
      {
         this->menu.activeMenu = P_ACTIVE_MENU_DEFINITION->nextMenu;
      }        
      //Parameter bearbeitung aktivieren
      else if(P_ACTIVE_MENU_DEFINITION->paramType == EDITABLE_PARAM__BOOL)
      {
         (*p_tempParamValue) = !(*p_tempParamValue);
      }
      else if(P_ACTIVE_MENU_DEFINITION->paramType == EDITABLE_PARAM__U8)
      {
         this->menu.f_editShownParameter = !this->menu.f_editShownParameter;
      }   
      Serial.println("PUSHED");
   }
  

   switch(ENCODER_DIRETION)
   {
      case MOVEMENT__LEFT: 
         if(this->menu.f_editShownParameter)
         {
            --(*p_tempParamValue);
         }
         else
         {
            this->menu.activeText--;
         }
         Serial.println("LEFT");
      break;

      case MOVEMENT__RIGHT: 
         if(this->menu.f_editShownParameter)
         {
            ++(*p_tempParamValue);
         }
         else
         {
            this->menu.activeText++;
         }
          Serial.println("RIGHT");
      break;
   }   
   //Menütexte begrenzen
   if(this->menu.activeText < 0)
   {
      this->menu.activeText = 0;
   }
   if(this->menu.activeText >= TEXT_COUNT_OF_ACTIVE_MENU)
   {
      this->menu.activeText = TEXT_COUNT_OF_ACTIVE_MENU;
   }

   //TEXT ausgabe
   s_oledStandartMenuPage_t   pageToShow;
   //Zeile 1  
   String* p_textRow1 = P_ACTIVE_MENU_DEFINITION->p_texts[ROW_1];    
   pageToShow.line[ROW_1].text = p_textRow1[ACTIVE_TEXT]; 
   //Zeile 2
   String* p_textRow2 = P_ACTIVE_MENU_DEFINITION->p_texts[ROW_2]; 
   pageToShow.line[ROW_2].f_blink = EDIT_PARAMETER;

   if(p_textRow2[ACTIVE_TEXT] == SHOW_PARAM_INSTEAD)
   {
      pageToShow.line[ROW_2].text   = String(*p_tempParamValue, DEC);
   }
   else
   {
      pageToShow.line[ROW_2].text   = p_textRow2[ACTIVE_TEXT];
   }     
   
   if(END_OF_MENU_REACHED)
   {
      if(this->menu.activeMenu != HMI_MENU__MAIN_MENU)
      {
         pageToShow.line[ROW_1].text = EXIT;
      }
      else
      {
         pageToShow.line[ROW_1].text = LOCK;
      }      
      pageToShow.line[ROW_2].text = PRESS;
      //Ende des Menü begrenzen 
      this->menu.activeText = TEXT_COUNT_OF_ACTIVE_MENU;
   }
   //Gebaute OLED menüseite ausgeben
   this->oledDisplay.setPage(pageToShow);
}