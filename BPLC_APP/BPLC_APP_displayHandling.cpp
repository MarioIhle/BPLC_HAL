#include "BPLC_APP.h"

const s_menu_t MENU_DEFINITION[5] = 
{
    //mainmenü
    {
        OLED_MENU__MAIN, 5, {"LOG", "DEVICE MODE", "BPLC STATE", "vDIP", "SETUP"}
    },
    //edit device Mode
    {
        OLED_MENU__DEVICE_MODE, 6, {"stop", "start", "safestate", "running C1", "running C2", "running C3"}
    },
     //edit vDips
    {
        OLED_MENU__VDIP, 8, {"vDIP1", "vDIP2", "vDIP3", "vDIP4", "vDIP5", "vDIP6", "vDIP7", "vDIP8"}
    },
    //bplc device hardware
    {
        OLED_MENU__BPLC_HARDWARE, 28,  {"DIN11 1",     "DIN11 2" ,     "DIN11 3",      "DIN11 4", //0-3
                                        "AIN11 1",     "AIN11 2" ,     "AIN11 3",      "AIN11 4", //4-7
                                        "DO11 1",      "DO11 2" ,      "DO11 3",       "DO11 4",  //8-11
                                        "REL11 1",     "REL11 2" ,     "REL11 3",      "REL11 4", //12-15
                                        "MOT11 1",     "MOT11 2" ,     "MOT11 3",      "MOT11 4", //16-19
                                        "FUSE11 1",    "FUSE11 2" ,    "FUSE11 3",     "FUSE11 4",//20-23
                                        "NANO11 1",    "NANO11 2" ,    "NANO11 3",     "NANO11 4"}//24-27
    },
    //edit device settings
    {
        OLED_MENU__DEVICE_SETTINGS, 5, {"SCREENSAVER", "MOT11_1 TEACHIN", "MOT11_2 TEACHIN", "MOT11_3 TEACHIN", "MOT11_4 TEACHIN"}
    },
};

void BPLC_APP::handleDisplay()
{  
    this->oled.tick();

    switch(this->displayHandling.activeMenu)
    {      
      case OLED_MENU__MAIN:
        this->showMainMenu();         
      break;

      case OLED_MENU__DEVICE_MODE:
        this->editDeviceMode();
      break;

      case OLED_MENU__BPLC_HARDWARE:
        this->hardwareState();
      break;

      case OLED_MENU__DEVICE_SETTINGS:
        this->displaySettings();        
      break;

      case OLED_MENU__VDIP:
        this->handle_vDip();
      break;

      case OLED_MENU__SCREENSAVER: 
         if(this->hal.ENCODER.isButtonPressed() || this->hal.ENCODER.getTurningDirection() != movement_idle)
         {
            this->displayHandling.activeMenu = OLED_MENU__MAIN;
         }
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

void BPLC_APP::showMainMenu()
{
    const bool           ENCODER_BUTTON_PRESSED     = this->hal.ENCODER.isButtonPressed();
    const e_movement_t   TURNING_DIRECTION          = this->hal.ENCODER.getTurningDirection();
    const bool           ENCODER_TURNED_LEFT        = (bool)(TURNING_DIRECTION == movement_left);
    const bool           ENCODER_TURNED_RIGHT       = (bool)(TURNING_DIRECTION == movement_right);

    if(ENCODER_BUTTON_PRESSED)
    {            
        this->displayHandling.activeMenu = (e_OLED_MENU_t)this->displayHandling.aktiveText;

        if(this->readyToExitMenu())
        {
            this->oled.showScreenSaver();
        }
    }

    if(ENCODER_TURNED_LEFT)
    {
        this->displayHandling.aktiveText++;
    }
    if(ENCODER_TURNED_RIGHT)
    {
        this->displayHandling.aktiveText--;
    }
}

void BPLC_APP::editDeviceMode()
{
    const bool           ENCODER_BUTTON_PRESSED     = this->hal.ENCODER.isButtonPressed();
    const e_movement_t   TURNING_DIRECTION          = this->hal.ENCODER.getTurningDirection();
    const bool           ENCODER_TURNED_LEFT        = (bool)(TURNING_DIRECTION == movement_left);
    const bool           ENCODER_TURNED_RIGHT       = (bool)(TURNING_DIRECTION == movement_right);

    const bool           PARAMETER_ENTERED          = this->displayHandling.f_editParameterOnDisplay;

   if(ENCODER_BUTTON_PRESSED)
   {                 
      if(PARAMETER_ENTERED)
      { //enter parameter       
        this->oled.blinkText(1, 1000);  
        this->displayHandling.f_editParameterOnDisplay = true;                        
      }
      else
      {//exit parameter
        this->deviceMode = (e_APP_MODE_t)this->displayHandling.tempValueToShow;
        this->displayHandling.f_editParameterOnDisplay = false;
      }
      //Cursor on "exit"
      if(this->readyToExitMenu())
      {
        this->displayHandling.activeMenu = OLED_MENU__MAIN;
      }      
   }

   if(ENCODER_TURNED_LEFT)
   {
      if(PARAMETER_ENTERED)
      {
        this->displayHandling.tempValueToShow++;
      }
      else
      {
        this->displayHandling.aktiveText++;    
      }
   }
   if(ENCODER_TURNED_RIGHT)
   {    
      if(PARAMETER_ENTERED)
      {
        this->displayHandling.tempValueToShow--;         
      }
      else
      {
        this->displayHandling.aktiveText--;         
      }
   }  

   //Bereichberenzung
   if(this->temp_ParameterStorage < (uint8_t)APP_MODE__STOP)
   {
      this->displayHandling.tempValueToShow = (uint8_t)APP_MODE__STOP;
   }
   if(this->displayHandling.tempValueToShow >= (uint8_t)APP_MODE__COUNT)
   {
      this->displayHandling.tempValueToShow = (uint8_t)APP_MODE__COUNT-1;
   }

    this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__MAIN].texts[OLED_MENU__DEVICE_MODE], 0);
    this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__DEVICE_MODE]->texts[this->displayHandling.tempValueToShow], 1)
   
   if(this->displayHandling.f_editParameterOnDisplay)
   {
      this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__DEVICE_MODE]->texts[this->displayHandling.tempValueToShow], 1)
   }
   else
   {
      this->oled.setTextToShow(MENU_DEFINITION[OLED_MENU__DEVICE_MODE]->texts[this->deviceMode], 1)
   }   
}

void BPLC_APP::hardwareState()
{

}

void BPLC_APP::displaySettings()
{
   if(this->hal.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.enterMenu(OLED_MENU__MAIN);         
      } 
      else
      {
         this->MOT11_CARD[MOT11_CARD__1].startCurrentAutotuning();
      }
   }

   if(this->hal.ENCODER.getTurningDirection() == movement_right)
   {
      this->oled.showNextTextOfThisMenu();
   }
   else if(this->hal.ENCODER.getTurningDirection() == movement_left)
   {
      this->oled.showPrevioursTextOfThisMenu();
   } 
}

void BPLC_APP::handle_vDip()
{  
    const bool           ENCODER_BUTTON_PRESSED     = this->hal.ENCODER.isButtonPressed();
    const e_movement_t   TURNING_DIRECTION          = this->hal.ENCODER.getTurningDirection();
    const bool           ENCODER_TURNED_LEFT        = (bool)(TURNING_DIRECTION == movement_left);
    const bool           ENCODER_TURNED_RIGHT       = (bool)(TURNING_DIRECTION == movement_right);
    const bool           PARARMETER_IS_ENTERED      = this->oled.parameterEntered();
    const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->oled.getActiveTextOfThisMenu();

    if(ENCODER_BUTTON_PRESSED)
    {            
        //Enter Parameter
        if(PARARMETER_IS_ENTERED == false)
        {           
            this->oled.enterParameter();  
            this->temp_ParameterStorage = this->getVDip(SELECTED_DIP);               
        }
        else
        {         
            this->setVDip(SELECTED_DIP, this->temp_ParameterStorage);  //Aktiver Text == gewählter Dip
            this->oled.exitParameter();               
        }
        //Cursor on "exit"
        if(this->oled.readyToExitMenu())
        {
            this->oled.enterMenu(OLED_MENU__MAIN);
        }  
    }

    if(TURNING_DIRECTION == movement_right)
    {
        if(PARARMETER_IS_ENTERED)
        {
            this->temp_ParameterStorage++;
        }
        else
        {
            this->oled.showNextTextOfThisMenu();   
        }
    }
    else if(TURNING_DIRECTION == movement_left)
    {    
        if(PARARMETER_IS_ENTERED)
        {
            this->temp_ParameterStorage--;
        }
        else
        {
            this->oled.showPrevioursTextOfThisMenu();         
        }
    }

    if(PARARMETER_IS_ENTERED)
    {
        this->oled.setParamValueToShow(this->temp_ParameterStorage);
    }
    else
    {
        this->oled.setParamValueToShow(this->getVDip(SELECTED_DIP));
    }
}
