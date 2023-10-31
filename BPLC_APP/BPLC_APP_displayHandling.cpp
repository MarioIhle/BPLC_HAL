#include "BPLC_APP.h"

//MENÜ STANDART TEXTE
const String EXIT       = "EXIT";
const String LOCK       = "LOCK";
const String LAST_TEXT  = "LAST";

const s_menu_t MENU_DEFINITION[6] = 
{
  //mainmenü
  {
    (uint8_t)OLED_MENU__MAIN, 6, {"EVENTLOG", "DEVICE MODE", "BPLC STATE", "MOT11 techIn", "vDIP", "SETUP", LOCK}
  },
  //edit device Mode
  {
    (uint8_t)OLED_MENU__DEVICE_MODE, 6, {"stop", "start", "safestate", "running C1", "running C2", "running C3", EXIT}
  },
  //edit vDips
  {
    (uint8_t)OLED_MENU__VDIP, 8, {"vDIP1", "vDIP2", "vDIP3", "vDIP4", "vDIP5", "vDIP6", "vDIP7", "vDIP8", EXIT}
  },
  //bplc device hardware states
  {
    (uint8_t)OLED_MENU__BPLC_HARDWARE_STATE, 28,  {"DIN11 1",     "DIN11 2" ,     "DIN11 3",      "DIN11 4", //0-3
                                          "AIN11 1",     "AIN11 2" ,     "AIN11 3",      "AIN11 4", //4-7
                                          "DO11 1",      "DO11 2" ,      "DO11 3",       "DO11 4",  //8-11
                                          "REL11 1",     "REL11 2" ,     "REL11 3",      "REL11 4", //12-15
                                          "MOT11 1",     "MOT11 2" ,     "MOT11 3",      "MOT11 4", //16-19
                                          "FUSE11 1",    "FUSE11 2" ,    "FUSE11 3",     "FUSE11 4",//20-23
                                          "NANO11 1",    "NANO11 2" ,    "NANO11 3",     "NANO11 4", //24-27
                                          EXIT}
  },                                      
  //bplc mot11 current autotuning starten
  {
    (uint8_t)OLED_MENU__MOT11_AUTOTUNING, 4, {"MOT11 1", "MOT11 2" , "MOT11 3", "MOT11 4", EXIT}
  },
  //edit device settings
  {
    (uint8_t)OLED_MENU__DEVICE_SETTINGS, 2, {{"SCREENSAVER"}, {"SLEEP TIME"}, {EXIT}}
  },
};

void BPLC_APP::displayBegin()
{
  this->oled.begin();
  this->screenSaverParameter.screenSaverIsEnbaled = true;
  this->screenSaverParameter.sleepTime = 60000;
  this->screenSaverParameter.to_sleep.setInterval(this->screenSaverParameter.sleepTime); 
  this->screenSaverParameter.to_sleep.reset(); 

  this->displayHandling.activeMenu = OLED_MENU__MAIN;
  this->displayHandling.activeText = 0;
  this->displayHandling.tempValueToShow = 0;

}

//Menü handling
void BPLC_APP::handleDisplay()
{  
  this->oled.tick();

  //Timeout abgelaufen 
  if(this->screenSaverParameter.to_sleep.check() && this->screenSaverParameter.screenSaverIsEnbaled == true)
  {
    this->displayHandling.activeMenu = OLED_MENU__SCREENSAVER;
  }
  //Solange userinput da ist, nicht in Screensaver
  if(this->hal.ENCODER.isButtonPressed() || this->hal.ENCODER.getTurningDirection() != movement_idle)
  {
    this->screenSaverParameter.to_sleep.reset(); 
  }

  switch(this->displayHandling.activeMenu)
  {      
    case OLED_MENU__MAIN:
      this->showMainMenu();         
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
      this->oled.clearAllTexts();
      this->screenSaverParameter.to_sleep.now();

      if(this->hal.ENCODER.isButtonPressed() || this->hal.ENCODER.getTurningDirection() != movement_idle)
      {
        this->screenSaverParameter.to_sleep.reset();
        this->displayHandling.activeMenu = OLED_MENU__BACK_TO_MAIN;        
      }
      break;

    default:
    case OLED_MENU__BACK_TO_MAIN:
      this->oled.clearAllTexts();
      this->displayHandling.activeMenu = OLED_MENU__MAIN;
      this->displayHandling.activeText = 0;
      break;
  }
}

bool BPLC_APP::readyToExitMenu()
{
  const uint8_t SHOWN_TEXT              = this->displayHandling.activeText; 
  const uint8_t LAST_TEXT_OF_THIS_MENU  = MENU_DEFINITION[(uint8_t)this->displayHandling.activeMenu].textCount;
  const bool    READY_TO_EXIT           = (bool)(SHOWN_TEXT == LAST_TEXT_OF_THIS_MENU); //Schon über bereich und daher wird exit angezeigt

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
  if((*P_VALUE_TO_EDIT) > MAX_VALUE)
  {
    (*P_VALUE_TO_EDIT) = MAX_VALUE;
  } 
}

//Menüs
void BPLC_APP::showMainMenu()
{
  const bool ENCODER_BUTTON_PRESSED = this->hal.ENCODER.isButtonPressed();

  if(ENCODER_BUTTON_PRESSED)
  {            
    this->displayHandling.activeMenu = (e_OLED_MENU_t)this->displayHandling.activeText;
    this->oled.clearAllTexts();

    if(this->readyToExitMenu())
    {
      this->displayHandling.activeMenu = OLED_MENU__SCREENSAVER;
    }
  }

  this->useEncoderForNavigation(&this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount);

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
}

void BPLC_APP::editDeviceMode()
{
  const bool           ENCODER_BUTTON_PRESSED = this->hal.ENCODER.isButtonPressed();
  const bool           PARAMETER_ENTERED      = this->displayHandling.f_editParameterOnDisplay;

  if(ENCODER_BUTTON_PRESSED)
  {                 
    if(PARAMETER_ENTERED)
    { //enter parameter       
      this->oled.startBlinkText(OLED_ROW__2, 500);  
      this->displayHandling.f_editParameterOnDisplay = true;                        
    }
    else
    {//exit parameter
     this->oled.stopBlinkText(OLED_ROW__2);  
      this->deviceMode = (e_APP_MODE_t)this->displayHandling.tempValueToShow;
      this->displayHandling.f_editParameterOnDisplay = false;
    }
    //Cursor on "exit"
    if(this->readyToExitMenu())
    {
      this->displayHandling.activeMenu = OLED_MENU__BACK_TO_MAIN;
    }      
  }

  if(PARAMETER_ENTERED)
  {
    this->useEncoderForNavigation(&this->displayHandling.tempValueToShow, (uint8_t)APP_MODE__COUNT);
  }
  else
  {
    this->useEncoderForNavigation(&this->displayHandling.activeText, MENU_DEFINITION[OLED_MENU__MAIN].textCount);
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
}

void BPLC_APP::hardwareState()
{

}

void BPLC_APP::mot11CurrentAutotuning()
{
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
    if(this->readyToExitMenu())
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
  
  if(this->readyToExitMenu())
  {
    this->oled.setTextToShow("", OLED_ROW__3);  
  }
  else
  {
    this->oled.setTextToShow("start", OLED_ROW__3);   
  } 
}

void BPLC_APP::displaySettings()
{
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
    if(this->readyToExitMenu())
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
  }  
}

void BPLC_APP::handle_vDip()
{  
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
    if(this->readyToExitMenu())
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
  }   
}
