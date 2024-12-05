#include "APP_HMI.h"

BPLC_HMI::BPLC_HMI()
{}

void BPLC_HMI::begin(hmiEncoder* P_HMI_ENCODER, output* P_BUZZER)
{
   this->p_mcuHmiEncoder = P_HMI_ENCODER;
   this->p_buzzer        = P_BUZZER;     
   this->oledDisplay.begin(); 
   this->menu.activeMenu = HMI_MENU__MAIN_MENU;
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
static String EXIT = "EXIT";
static String press = "press";
static String SHOW_PARAM_INSTEAD = "param";

String screenSaver[2][1] =   
{  
   {{""}},
   {{""}}
};
String MainMenuTexts[2][HMI_MENU__COUNT] =   
{  
   {{"DEV MODE"},             {"EDIT MODE"}, {"ERROR"},  {"VDIP"},   {"SETTINGS"}},
   {SHOW_PARAM_INSTEAD,        press,         press,      press,      press}
};
String editDeviceModeTexts[2][BPLC_DEVICE_MODE__COUNT] =   
{  
   {"EDIT MODE"},
   {{"stop"},{"start"},{"safe"},{"run"},{"run no error"},{"run no hal"},{"run no com"}}
};
String errorOutTexts[2][1] =   
{  
   {"ERROR CODE"},
   {SHOW_PARAM_INSTEAD}
};



s_menu_t menuDefinition[HMI_MENU__COUNT] =  
{
   //{p_texts,                      menuPages,                 f_hasParam,    nextMenu}
   {&screenSaver[0][0],             0,                         EDITABLE_PARAM__NO,         HMI_MENU__MAIN_MENU},
   {&MainMenuTexts[0][0],           HMI_MENU__COUNT,           EDITABLE_PARAM__NO,         HMI_MENU__SCREENSAVER},
   {&editDeviceModeTexts[0][0],     BPLC_DEVICE_MODE__COUNT,   EDITABLE_PARAM__U8,         HMI_MENU__MAIN_MENU},
   {&errorOutTexts[0][0],           1,                         EDITABLE_PARAM__BOOL,       HMI_MENU__MAIN_MENU},
   {&errorOutTexts[0][0],           1,                         EDITABLE_PARAM__BOOL,       HMI_MENU__MAIN_MENU},
};
               

void BPLC_HMI::showMenu(uint8_t* p_tempParamValue)
{   
   const e_HMI_MENU_t         ACTIVE_MENU                = this->menu.activeMenu;
   const uint8_t              ACTIVE_TEXT                = this->menu.activeText;   

   const s_menu_t*            P_ACTIVE_MENU_DEFINITION   = &menuDefinition[ACTIVE_MENU];
   const uint8_t              TEXT_COUNT_OF_ACTIVE_MENU  = P_ACTIVE_MENU_DEFINITION->menuPages;
   const bool                 END_OF_MENU_REACHED        = (ACTIVE_TEXT > TEXT_COUNT_OF_ACTIVE_MENU);
   const bool                 EDIT_PARAMETER             = ((P_ACTIVE_MENU_DEFINITION->paramType!= EDITABLE_PARAM__NO) && this->menu.f_editShownParameter);

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
      break;
   }   

   //TEXT ausgabe
   s_oledStandartMenuPage_t   pageToShow; 

   pageToShow.line[ROW_1].text    = MainMenuTexts[ROW_1][ACTIVE_TEXT];  

   if(MainMenuTexts[ROW_2][ACTIVE_TEXT] == SHOW_PARAM_INSTEAD)
   {
      pageToShow.line[ROW_2].text   = String(*p_tempParamValue, DEC);
   }
   else
   {
      pageToShow.line[ROW_2].text   = P_ACTIVE_MENU_DEFINITION->p_texts[ROW_2][ACTIVE_TEXT];
   }

   Serial.println(pageToShow.line[ROW_1].text );
   Serial.println(pageToShow.line[ROW_2].text );   

   pageToShow.line[ROW_2].f_blink = EDIT_PARAMETER;

   if(END_OF_MENU_REACHED)
   {
      if(this->menu.activeMenu != HMI_MENU__MAIN_MENU)
      {
         pageToShow.line[ROW_1].text = "EXIT";
      }
      else
      {
         pageToShow.line[ROW_1].text = "LOCK";
      }      
      pageToShow.line[ROW_2].text = "press";
      //Ende des Menü begrenzen 
      this->menu.activeText = TEXT_COUNT_OF_ACTIVE_MENU;
   }
   //Gebaute OLED menüseite ausgeben
   this->oledDisplay.setPage(pageToShow);
}



/*

void BPLC_HMI::showMainMenu(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
{  
   this->oledDisplay.setPage(pageToShow);

   //Menüsteuerung
   if(ENCODER_BUTTON_PRESSED)
   {            
      this->menu.activeMenu = (e_HMI_MENU_t)this->menu.activeText;
   }

   if(ENCODER_DIRETION == MOVEMENT__RIGHT)
   {
      this->menu.activeText++;
   }
   else if(ENCODER_DIRETION == MOVEMENT__LEFT)
   {
      this->menu.activeText--;
   }  
   
   //Menüausgabe
   this->showMenu();
}
void BPLC_HMI::editDeviceMode(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
{
   if(ENCODER_BUTTON_PRESSED)
   {                 
      //Enter Parameter
      if(this->APP_HAL.oled.parameterEntered() == false)
      {           
         this->APP_HMI.temp_ParameterStorage = this->APP_APP.deviceMode;
         this->APP_HAL.oled.enterParameter();                          
      }
      else
      {
         this->setDeviceMode((e_BPLC_DEVICE_MODE_t)this->APP_HMI.temp_ParameterStorage);
         this->APP_HAL.oled.exitParameter();
      }
      //Cursor on "exit"
      if(this->APP_HAL.oled.readyToExitMenu())
      {
         this->APP_HAL.oled.setMenu(menu_mainMenu);
      }      
   }

   if(ENCODER_DIRETION == MOVEMENT__RIGHT)
   {
      if(this->APP_HAL.oled.parameterEntered())
      {
         this->APP_HMI.temp_ParameterStorage++;
      }
      else
      {
         this->APP_HAL.oled.showNextTextOfThisMenu();            
      }
   }
   else if(ENCODER_DIRETION == MOVEMENT__LEFT)
   {    
      if(this->APP_HAL.oled.parameterEntered())
      {
         this->APP_HMI.temp_ParameterStorage--;         
      }
      else
      {
         this->APP_HAL.oled.showPrevioursTextOfThisMenu();            
      }
   }  

   //Bereichberenzung
   if(this->APP_HMI.temp_ParameterStorage < BPLC_DEVICE_MODE__STOP)
   {
      APP_HMI.temp_ParameterStorage = BPLC_DEVICE_MODE__STOP;
   }
   if(this->APP_HMI.temp_ParameterStorage >= BPLC_DEVICE_MODE__COUNT)
   {
      this->APP_HMI.temp_ParameterStorage = BPLC_DEVICE_MODE__COUNT-1;
   }

   if(this->APP_HAL.oled.parameterEntered())
   {
      this->APP_HAL.oled.showPage(this->APP_HMI.temp_ParameterStorage);
   }
   else
   {
      this->APP_HAL.oled.showPage(this->APP_APP.deviceMode);
   }   
}
void BPLC_HMI::showErrorOut(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
{
   const uint8_t ERROR_CODE_DISPLAYED = (this->APP_HAL.oled.getActiveMenuTextNum() == 0);

   if(ENCODER_BUTTON_PRESSED)
   {            
      //Cursor on "exit"
      if(this->APP_HAL.oled.readyToExitMenu())
      {
         this->APP_HAL.oled.setMenu(menu_mainMenu);         
      } 
      else if(ERROR_CODE_DISPLAYED)
      {
         this->systemErrorManager.resetAllErrors(__FILENAME__, __LINE__);
      }
   }
   uint8_t ERROR_CODE = (uint8_t)this->systemErrorManager.getError()->errorCode;

   this->APP_HAL.oled.showPage(ERROR_CODE);
   
   if(ENCODER_DIRETION == MOVEMENT__RIGHT)
   {
      this->APP_HAL.oled.showNextTextOfThisMenu();
   }
   else if(ENCODER_DIRETION == MOVEMENT__LEFT)
   {
      this->APP_HAL.oled.showPrevioursTextOfThisMenu();
   }    
}
void BPLC_HMI::displaySettings(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
{
   const uint8_t DISPLAYED_SETTING = this->APP_HAL.oled.getActiveMenuTextNum();

   if(ENCODER_BUTTON_PRESSED)
   {            
      //Cursor on "exit"
      if(this->APP_HAL.oled.readyToExitMenu())
      {
         this->APP_HAL.oled.setMenu(menu_mainMenu);         
      } 
      else if(DISPLAYED_SETTING == 0)
      {         
         this->APP_APP.settings.device.application.f_useBuzzer = (!this->APP_APP.settings.device.application.f_useBuzzer);
      }
      else if(DISPLAYED_SETTING == 1)
      {
         //this->APP_HAL.MOT11_CARD[MOT11_CARD__1].startCurrentAutotuning();
      }
   }

   if (DISPLAYED_SETTING == 0)
   {
      this->APP_HAL.oled.showPage(this->APP_APP.settings.device.application.f_useBuzzer);
   }

   if(ENCODER_DIRETION == MOVEMENT__RIGHT)
   {
      this->APP_HAL.oled.showNextTextOfThisMenu();
   }
   else if(ENCODER_DIRETION == MOVEMENT__LEFT)
   {
      this->APP_HAL.oled.showPrevioursTextOfThisMenu();
   } 
}
void BPLC_HMI::handle_vDip(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
{  
   const bool           IS_ENCODER_BUTTON_PRESSED  = ENCODER_BUTTON_PRESSED;
   const e_MOVEMENT_t   TURNING_DIRECTION          = ENCODER_DIRETION;
   const bool           PARARMETER_IS_ENTERED      = this->APP_HAL.oled.parameterEntered();
   const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->APP_HAL.oled.getActiveMenuTextNum();

   if(IS_ENCODER_BUTTON_PRESSED)
   {            
      //Enter Parameter
      if(PARARMETER_IS_ENTERED == false)
      {           
         this->APP_HAL.oled.enterParameter();  
         this->APP_HMI.temp_ParameterStorage = this->getVDip(SELECTED_DIP);               
      }
      else
      {         
         this->setVDip(SELECTED_DIP, this->APP_HMI.temp_ParameterStorage);  //Aktiver Text == gewählter Dip
         this->APP_HAL.oled.exitParameter();               
      }
      //Cursor on "exit"
      if(this->APP_HAL.oled.readyToExitMenu())
      {
         this->APP_HAL.oled.setMenu(menu_mainMenu);
      }  
   }

   if(TURNING_DIRECTION == MOVEMENT__RIGHT)
   {
      if(PARARMETER_IS_ENTERED)
      {
         this->APP_HMI.temp_ParameterStorage++;
      }
      else
      {
         this->APP_HAL.oled.showNextTextOfThisMenu();   
      }
   }
   else if(TURNING_DIRECTION == MOVEMENT__LEFT)
   {    
      if(PARARMETER_IS_ENTERED)
      {
         this->APP_HMI.temp_ParameterStorage--;
      }
      else
      {
         this->APP_HAL.oled.showPrevioursTextOfThisMenu();         
      }
   }

   if(PARARMETER_IS_ENTERED)
   {
      this->APP_HAL.oled.showPage(this->APP_HMI.temp_ParameterStorage);
   }
   else
   {
      this->APP_HAL.oled.showPage(this->getVDip(SELECTED_DIP));
   }

#ifdef DEBUG_APP_MCU11_APP_HMI
for(int i = 0; i< vDIP_COUNT; i++)
{
   Serial.print(", DIP "); Serial.print(i+1); Serial.print(": "); Serial.print(this->virtualDipSwitch[i]);
}
Serial.println("");
#endif
}






















//old 

//---------------------------------------------------
// MENU AUSGABE
// Texte 2. Zeile
String DEVICE_MODE[] = {{"stop"}, {"start"}, {"safestate"}, {"running"}, {"running no safety"}, {"running no HAL"}, {"running no COM"}};

void OLED_STANDART_MENU::showMainMenu()
{
  if (this->menu.activeText == 0)
  {
    this->showMenuText(DEVICE_MODE[this->paramValue], 1);
  }
  else
  {
    this->showMenuText("", 1);
  }
}

void OLED_STANDART_MENU::showHardwareErrorCode()
{
  if (this->menu.activeText == 0)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
  else
  {
    this->showMenuText("", 1);
  }  
}

void OLED_STANDART_MENU::showSettings()
{
  if (this->menu.activeText == 0)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
  else
  {
    this->showMenuText("", 1);
  }  
}

void OLED_STANDART_MENU::showDipswitches()
{
  if (this->menu.activeText < 8)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
}

void OLED_STANDART_MENU::showDeviceMode()
{
  this->showMenuText(DEVICE_MODE[this->paramValue], 1);
}

//---------------------------------------------------
// Parametermode
void OLED_STANDART_MENU::enterParameter()
{
  // Nur bei Parameter in Parametriermode
  if (HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] != EXIT)
  {
    this->f_parmParameter = true;
  }
}

void OLED_STANDART_MENU::showPage(const uint8_t VALUE)
{
  this->paramValue = VALUE;
}

void OLED_STANDART_MENU::exitParameter()
{
  this->f_parmParameter = false;
  this->f_refresh = true;
}

bool OLED_STANDART_MENU::parameterEntered()
{
  return this->f_parmParameter;
}

e_BPLC_ERROR_t  OLED_STANDART_MENU::getError()
{
  return this->errorCode;
}*/