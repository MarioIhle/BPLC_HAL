#include "BPLC_APP.h"

//MENÜ STANDART TEXTE
const String EXIT       = "EXIT";
const String LOCK       = "LOCK";
const String LAST_TEXT  = "LAST";
uint8_t NO_VAR = 123;

void BPLC_APP::displayBegin()
{
  this->oled.begin(&this->hal.ENCODER);
  
  this->displayHandling.activeMenu = OLED_MENU__MAIN;
  this->displayHandling.activeText = 0;
  this->displayHandling.tempValueToShow = 0; 
  //Main menu
  this->MENU_DEFINITION[OLED_MENU__MAIN].name                  = (uint8_t)OLED_MENU__MAIN;
  this->MENU_DEFINITION[OLED_MENU__MAIN].nameOfMenuWhenExiting = (uint8_t)OLED_MENU__SCREENSAVER;
  this->MENU_DEFINITION[OLED_MENU__MAIN].textCount             = 5;
  this->MENU_DEFINITION[OLED_MENU__MAIN].headline              = "BPLC V1.0";
  this->MENU_DEFINITION[OLED_MENU__MAIN].p_VAR                 =  &NO_VAR;
  this->MENU_DEFINITION[OLED_MENU__MAIN].subHeadline[0]        = "edit device mode";
  this->MENU_DEFINITION[OLED_MENU__MAIN].subHeadline[1]        = "bplc cards";
  this->MENU_DEFINITION[OLED_MENU__MAIN].subHeadline[2]        = "edit device mode";
  //Device Mode
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].name                  = (uint8_t)OLED_MENU__MAIN;
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].nameOfMenuWhenExiting = (uint8_t)OLED_MENU__SCREENSAVER;
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].textCount             = 2;
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].headline              = "headline";
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].p_VAR                 =  &NO_VAR;
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].subHeadline[0]        = "subHedlin1";
  this->MENU_DEFINITION[OLED_MENU__DEVICE_MODE].subHeadline[1]        = "subHedlin2";
}

/*
//mainmenü
  {
    (uint8_t)OLED_MENU__MAIN, 
    (uint8_t)OLED_MENU__SCREENSAVER, 
    6, 
    "MAINMENU", 
    {"EVENTLOG", "DEVICE MODE", "BPLC STATE", "MOT11 techIn", "vDIP", "SETUP"}    ,
    &this->deviceMode,      
    "<<scroll>>"
  }; 
    //edit device Mode
    {
      (uint8_t)OLED_MENU__DEVICE_MODE,    
      (uint8_t)OLED_MENU__MAIN, 
      6,
      "DEV MODE",      
      {"stop", "start", "safestate", "running C1", "running C2", "running C3"},
      &this->deviceMode,      
      "press"      
    },
    
    //edit vDips
    {
      (uint8_t)OLED_MENU__VDIP, 
      (uint8_t)OLED_MENU__BACK_TO_MAIN, 
      "vDIPs",
      8, 
      {"vDIP1", "vDIP2", "vDIP3", "vDIP4", "vDIP5", "vDIP6", "vDIP7", "vDIP8"},
      &this->deviceMode,      
      "press"
    },
    //bplc device hardware states
    {
      (uint8_t)OLED_MENU__BPLC_HARDWARE_STATE, 
      (uint8_t)OLED_MENU__BACK_TO_MAIN, 
      "BPLC CARDS",
      28,  
      {
        "DIN11 1",     "DIN11 2" ,     "DIN11 3",      "DIN11 4", //0-3
        "AIN11 1",     "AIN11 2" ,     "AIN11 3",      "AIN11 4", //4-7
        "DO11 1",      "DO11 2" ,      "DO11 3",       "DO11 4",  //8-11
        "REL11 1",     "REL11 2" ,     "REL11 3",      "REL11 4", //12-15
        "MOT11 1",     "MOT11 2" ,     "MOT11 3",      "MOT11 4", //16-19
        "FUSE11 1",    "FUSE11 2" ,    "FUSE11 3",     "FUSE11 4",//20-23
        "NANO11 1",    "NANO11 2" ,    "NANO11 3",     "NANO11 4", //24-27
      }
      &this->deviceMode,      
      "press"
    },                                      
    //bplc mot11 current autotuning starten
    {
      (uint8_t)OLED_MENU__MOT11_AUTOTUNING, 
      (uint8_t)OLED_MENU__BACK_TO_MAIN, 
      "MOT11 TEACH",
      4, 
      {"MOT11 1", "MOT11 2" , "MOT11 3", "MOT11 4"},
      &this->deviceMode,      
      "press"
    },
    //edit device settings
    {
      (uint8_t)OLED_MENU__DEVICE_SETTINGS, 
      (uint8_t)OLED_MENU__BACK_TO_MAIN, 
      "SETTINGS",
      2, 
      {"SCREENSAVER", "SLEEP TIME"},
      &this->deviceMode,      
      "press"
    }*/
//Menü handling
void BPLC_APP::handleDisplay()
{  
  this->oled.tick(); 

  switch(this->displayHandling.activeMenu)
  {      
    case OLED_MENU__MAIN:
      showAnyDefinedMenu(this->MENU_DEFINITION[OLED_MENU__MAIN], "123");
      break;

    case OLED_MENU__DEVICE_MODE:
      this->editDeviceMode();
      break;

    case OLED_MENU__VDIP:
      this->handle_vDip();
      break;

    case OLED_MENU__BPLC_HARDWARE_STATE:
      this->hardwareState();
      break;

    case OLED_MENU__MOT11_AUTOTUNING:
      this->mot11CurrentAutotuning();
      break;

    case OLED_MENU__DEVICE_SETTINGS:
      this->displaySettings();        
      break;

    case OLED_MENU__SCREENSAVER:     
        this->oled.showScreensaver();
        this->displayHandling.activeMenu = OLED_MENU__BACK_TO_MAIN;      
      break;

    default:
    case OLED_MENU__BACK_TO_MAIN:
      this->oled.clearAllTexts();
      this->displayHandling.activeMenu = OLED_MENU__MAIN;
      this->displayHandling.activeText = 0;
      break;
  }
}

bool BPLC_APP::readyToExitMenu(const uint8_t SHOWN_TEXT, const uint8_t LAST_TEXT_OF_THIS_MENU)
{
  const bool READY_TO_EXIT = (bool)(SHOWN_TEXT >= LAST_TEXT_OF_THIS_MENU); //Schon über bereich und daher wird exit angezeigt

  return READY_TO_EXIT;
}

void BPLC_APP::useEncoderForNavigation(uint8_t* P_VALUE_TO_EDIT, const uint8_t MAX_VALUE)
{
  const e_movement_t   TURNING_DIRECTION          = this->hal.ENCODER.getTurningDirection();
  const bool           ENCODER_TURNED_LEFT        = (bool)(TURNING_DIRECTION == movement_left);
  const bool           ENCODER_TURNED_RIGHT       = (bool)(TURNING_DIRECTION == movement_right);
  
  if(ENCODER_TURNED_LEFT)
  {
    (*P_VALUE_TO_EDIT)--;
  }
  if(ENCODER_TURNED_RIGHT)
  {  
    (*P_VALUE_TO_EDIT)++;
  }
 
  //Bereichberenzung
  if((*P_VALUE_TO_EDIT) == (uint8_t)255)
  {
    (*P_VALUE_TO_EDIT) = MAX_VALUE;
  } 
  else if((*P_VALUE_TO_EDIT) > MAX_VALUE)
  {
    (*P_VALUE_TO_EDIT) = (uint8_t)0;
  } 
}

//Menüs
void BPLC_APP::showAnyDefinedMenu(s_menu_t MENU, String VAR)
{
  const bool ENCODER_BUTTON_PRESSED = this->hal.ENCODER.isButtonPressed();
  const bool READY_TO_EXIT_MENU     = (bool)(this->displayHandling.activeText >= MENU.textCount);

  this->useEncoderForNavigation(&this->displayHandling.activeText, MENU.textCount);

  if(ENCODER_BUTTON_PRESSED)
  {            
    this->displayHandling.activeMenu = (e_OLED_MENU_t)MENU.nameOfMenuWhenExiting;
    this->oled.clearAllTexts();    
  }

  //Textausgabe
  this->oled.setTextToShow(MENU.headline, OLED_ROW__1);

  if(READY_TO_EXIT_MENU)
  {
    this->oled.setTextToShow("exit", OLED_ROW__2);
  } 
  else
  {
    this->oled.setTextToShow(MENU.subHeadline[this->displayHandling.activeText], OLED_ROW__2);

    if(VAR != "NO_VAR_TO_SHOW")
    {  
      this->oled.setTextToShow(VAR, OLED_ROW__3);
    }  
    else
    {
      this->oled.setTextToShow("", OLED_ROW__3);    
    }
  } 
}


void BPLC_APP::showMainMenu()
{
  /*
  const bool ENCODER_BUTTON_PRESSED = this->hal.ENCODER.isButtonPressed();
  const bool READY_TO_EXIT_MENU     = this->readyToExitMenu(this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount);

  this->useEncoderForNavigation(&this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount);

  if(READY_TO_EXIT_MENU)
  {
    this->oled.setTextToShow(EXIT, OLED_ROW__1);   

    if(ENCODER_BUTTON_PRESSED)
    {
      this->displayHandling.activeMenu = (e_OLED_MENU_t)MENU_DEFINITION[OLED_MENU__MAIN].nameOfMenuWhenExiting;
      this->oled.clearAllTexts();       
    }      
  }
  else
  {
    if(ENCODER_BUTTON_PRESSED)
    {            
      this->displayHandling.activeMenu = (e_OLED_MENU_t)this->displayHandling.activeText;
    }
    //Texte an oled übergeben
    this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__MAIN].texts[this->displayHandling.activeText], OLED_ROW__1); 
    if(this->displayHandling.activeText == 0)
    {
      this->oled.setTextToShow("1. EVENT", OLED_ROW__2);   
      this->oled.setTextToShow("2. EVENT", OLED_ROW__3);   
      this->oled.setTextToShow("3. EVENT", OLED_ROW__4);   
    }
    else
    {
      this->oled.setTextToShow("", OLED_ROW__2);   
      this->oled.setTextToShow("", OLED_ROW__3);   
      this->oled.setTextToShow("", OLED_ROW__4); 
    }
  }*/
}

void BPLC_APP::editDeviceMode()
{
  /*
  const bool ENCODER_BUTTON_PRESSED = this->hal.ENCODER.isButtonPressed();
  const bool PARAMETER_ENTERED      = this->displayHandling.f_editParameterOnDisplay;
  const bool TEXT_TO_SHOW           = this->displayHandling.activeText;
  const bool READY_TO_EXIT_MENU     = (bool)(TEXT_TO_SHOW >= 2 && PARAMETER_ENTERED == false);

  if(PARAMETER_ENTERED)
  {
    this->useEncoderForNavigation(&this->displayHandling.tempValueToShow, MENU_DEFINITION[OLED_MENU__DEVICE_MODE].textCount);
  }
  else
  {
    this->useEncoderForNavigation(&this->displayHandling.activeText, (uint8_t)2);
  }
  
  if(READY_TO_EXIT_MENU)
  {
    this->oled.setTextToShow(EXIT, OLED_ROW__1);   

    if(ENCODER_BUTTON_PRESSED)
    {
      this->displayHandling.activeMenu = (e_OLED_MENU_t)MENU_DEFINITION[OLED_MENU__DEVICE_MODE].nameOfMenuWhenExiting;
      this->oled.clearAllTexts();     
      this->displayHandling.activeText = 0;
      this->displayHandling.tempValueToShow = 0;  
    }      
  }
  else
  {
    if(ENCODER_BUTTON_PRESSED)
    {
      if(PARAMETER_ENTERED)
      {//exit parameter
        this->oled.stopBlinkText(OLED_ROW__3);  
        this->deviceMode = (e_APP_MODE_t)this->displayHandling.tempValueToShow;
        this->displayHandling.f_editParameterOnDisplay = false;                     
      }
      else
      {//enter parameter       
        this->oled.startBlinkText(OLED_ROW__3, 500);  
        this->displayHandling.f_editParameterOnDisplay = true;   
      }
    }

    //Texte an oled übergeben
    this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__MAIN].texts[OLED_MENU__DEVICE_MODE], OLED_ROW__1);
    
    if(PARAMETER_ENTERED)
    {//Temporärer Wert ausgeben, nicht direkt "Golbale" Variable bearbeiten
      this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__DEVICE_MODE].texts[this->displayHandling.tempValueToShow], OLED_ROW__3);
    }
    else
    {
      this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__DEVICE_MODE].texts[this->deviceMode], OLED_ROW__3);
    } 
  }  */
}

void BPLC_APP::hardwareState()
{

}

void BPLC_APP::mot11CurrentAutotuning()
{
 /*
  const bool    ENCODER_BUTTON_PRESSED  = this->hal.ENCODER.isButtonPressed();
  const bool    PARAMETER_ENTERED       = this->displayHandling.f_editParameterOnDisplay;
  const uint8_t SELECTED_MOT11_CARD     = (e_MOT11_CARD_t)this->displayHandling.activeText;

  if(ENCODER_BUTTON_PRESSED)
  {  
    if(PARAMETER_ENTERED)
    { //enter parameter       
      this->MOT11_CARD[SELECTED_MOT11_CARD].startCurrentAutotuning();        
    }    
    //Cursor on "exit"
    if(this->readyToExitMenu(this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount))
    {
      this->displayHandling.activeMenu = OLED_MENU__BACK_TO_MAIN;
    } 
  }

  //Text blinken solange Autotuning läuft
  if(this->MOT11_CARD[SELECTED_MOT11_CARD].getDeviceState() == MOT11_DEVICE_STATE__CURRENT_TEACH_IN)
  {
    this->oled.startBlinkText(OLED_ROW__3, 250);  
  }
  else
  {
    this->oled.stopBlinkText(OLED_ROW__3);  
  }  

  //Texte an oled übergeben
  this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__MAIN].texts[OLED_MENU__MOT11_AUTOTUNING], OLED_ROW__1);   
  this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__MOT11_AUTOTUNING].texts[SELECTED_MOT11_CARD], OLED_ROW__2);   
  
  if(this->readyToExitMenu(this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount))
  {
    this->oled.setTextToShow("", OLED_ROW__3);  
  }
  else
  {
    this->oled.setTextToShow("start", OLED_ROW__3);   
  } */
}

void BPLC_APP::displaySettings()
{
  /*
  const bool           ENCODER_BUTTON_PRESSED = this->hal.ENCODER.isButtonPressed();
  const bool           PARAMETER_ENTERED      = this->displayHandling.f_editParameterOnDisplay;

  if(ENCODER_BUTTON_PRESSED)
  {  
    if(PARAMETER_ENTERED)
    { //enter parameter       
      this->oled.startBlinkText(OLED_ROW__3, 500);  
      this->displayHandling.f_editParameterOnDisplay = true;                        
    }
    else
    {//exit parameter
      this->oled.stopBlinkText(OLED_ROW__3);
      this->screenSaverParameter.sleepTime = this->displayHandling.tempValueToShow*1000;
      this->displayHandling.f_editParameterOnDisplay = false;
    }
    //Cursor on "exit"
    if(this->readyToExitMenu(this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount))
    {
      this->displayHandling.activeMenu = OLED_MENU__BACK_TO_MAIN;
    } 
  }

  if(PARAMETER_ENTERED)
  {
    this->useEncoderForNavigation(&this->displayHandling.tempValueToShow, 255);
  }
  else
  {
    this->useEncoderForNavigation(&this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__DEVICE_SETTINGS].textCount);
  }

  //Texte an oled übergeben
  this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__MAIN].texts[OLED_MENU__DEVICE_SETTINGS], OLED_ROW__1);
  
  if(PARAMETER_ENTERED)
  {//Temporärer Wert ausgeben, nicht direkt "Golbale" Variable bearbeiten
    this->oled.setTextToShow((String)(this->displayHandling.tempValueToShow * 1000), OLED_ROW__3);
  }
  else
  {
    this->oled.setTextToShow((String)(this->screenSaverParameter.sleepTime * 1000), OLED_ROW__3);
  }  */
}

void BPLC_APP::handle_vDip()
{  
 /*
  const bool           ENCODER_BUTTON_PRESSED     = this->hal.ENCODER.isButtonPressed();
  const bool           PARAMETER_ENTERED          = this->displayHandling.f_editParameterOnDisplay;
  const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->displayHandling.activeText;

  if(ENCODER_BUTTON_PRESSED)
  {            
    //Enter Parameter
    if(PARAMETER_ENTERED == false)
    {           
      this->oled.startBlinkText(OLED_ROW__3, 500);  
      this->displayHandling.f_editParameterOnDisplay = true;
      this->displayHandling.tempValueToShow = this->getVDip(SELECTED_DIP);               
    }
    else
    {         
      this->oled.stopBlinkText(OLED_ROW__3);  
      this->displayHandling.f_editParameterOnDisplay = false;
      this->setVDip(SELECTED_DIP, this->displayHandling.tempValueToShow);               
    }
    //Cursor on "exit"
    if(this->readyToExitMenu(this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount))
    {
      this->displayHandling.activeMenu = OLED_MENU__BACK_TO_MAIN;
    }  
  }
  
  if(PARAMETER_ENTERED)
  {//Dip Wert bearbeiten
    this->useEncoderForNavigation(&this->displayHandling.tempValueToShow, 255);
  }
  else
  {//DIP wählen
    this->useEncoderForNavigation(&this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__VDIP].textCount);
  }

  //Texte an oled übergeben
  this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__VDIP].texts[SELECTED_DIP], OLED_ROW__1);
  
  if(PARAMETER_ENTERED)
  {//Temporärer Wert ausgeben, nicht direkt "Golbale" Variable bearbeiten
    this->oled.setTextToShow((String)this->displayHandling.tempValueToShow, OLED_ROW__3);
  }
  else
  {
    this->oled.setTextToShow((String)this->getVDip(SELECTED_DIP), OLED_ROW__3);
  }   */
}
