#include"BPLC_APP.h"

void BPLC_APP::handleDisplay()
{  
   switch(this->oled.getActiveMenu())
   {      
      case menu_mainMenu:
         if(this->APP_HAL.ENCODER.isButtonPressed())
         {            
            this->oled.enterMenu();            
         }

         if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
         {
            this->oled.showNextTextOfThisMenu();
         }
         else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
         {
            this->oled.showPrevioursTextOfThisMenu();
         }           
         this->oled.setParamValueToShow(this->deviceMode);
      break;

      case menu_deviceMode:
         editDeviceMode();
      break;

      case menu_errorCodes:
         hardwareErrorOut();
      break;

      case menu_settings:
         displaySettings();        
      break;

      case menu_dipSwitch:
         handle_vDip();
      break;

      case menu_screenSaver: 
         if(this->APP_HAL.ENCODER.isButtonPressed() || this->APP_HAL.ENCODER.getTurningDirection() != movement_idle)
         {
            this->oled.setMenu(menu_mainMenu);
         }
      break;
   }
}

//Display handling
void BPLC_APP::editDeviceMode()
{
   if(this->APP_HAL.ENCODER.isButtonPressed())
   {                 
      //Enter Parameter
      if(this->oled.parameterEntered() == false)
      {           
         this->temp_ParameterStorage = this->deviceMode;
         this->oled.enterParameter();                          
      }
      else
      {
         this->deviceMode = (e_APP_MODE_t)this->temp_ParameterStorage;
         this->oled.exitParameter();
      }
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);
      }      
   }

   if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
   {
      if(this->oled.parameterEntered())
      {
         this->temp_ParameterStorage++;
      }
      else
      {
         this->oled.showNextTextOfThisMenu();            
      }
   }
   else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
   {    
      if(this->oled.parameterEntered())
      {
         this->temp_ParameterStorage--;         
      }
      else
      {
         this->oled.showPrevioursTextOfThisMenu();            
      }
   }  

   //Bereichberenzung
   if(this->temp_ParameterStorage < APP_MODE__STOP)
   {
      temp_ParameterStorage = APP_MODE__STOP;
   }
   if(this->temp_ParameterStorage >= APP_MODE__COUNT)
   {
      this->temp_ParameterStorage = APP_MODE__COUNT-1;
   }

   if(this->oled.parameterEntered())
   {
      this->oled.setParamValueToShow(this->temp_ParameterStorage);
   }
   else
   {
      this->oled.setParamValueToShow(this->deviceMode);
   }   
}

void BPLC_APP::hardwareErrorOut()
{
   if(this->APP_HAL.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);         
      } 
   }

   if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
   {
      this->oled.showNextTextOfThisMenu();
   }
   else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
   {
      this->oled.showPrevioursTextOfThisMenu();
   } 

   this->oled.setParamValueToShow(this->hardwareErrorCode[0]);    
}

void BPLC_APP::displaySettings()
{
   if(this->APP_HAL.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);         
      } 
      else
      {
         this->APP_HAL.MOT11_CARD[MOT11_CARD__1].startCurrentAutotuning();
      }
   }

   if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
   {
      this->oled.showNextTextOfThisMenu();
   }
   else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
   {
      this->oled.showPrevioursTextOfThisMenu();
   } 
}

void BPLC_APP::handle_vDip()
{  
   const bool           IS_ENCODER_BUTTON_PRESSED  = this->APP_HAL.ENCODER.isButtonPressed();
   const e_movement_t   TURNING_DIRECTION          = this->APP_HAL.ENCODER.getTurningDirection();
   const bool           PARARMETER_IS_ENTERED      = this->oled.parameterEntered();
   const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->oled.getActiveMenuTextNum();

   if(IS_ENCODER_BUTTON_PRESSED)
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
         this->oled.setMenu(menu_mainMenu);
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

#ifdef DEBUG_APP_MCU11_OLED_HANDLING
for(int i = 0; i< vDIP_COUNT; i++)
{
   Serial.print(", DIP "); Serial.print(i+1); Serial.print(": "); Serial.print(this->virtualDipSwitch[i]);
}
Serial.println("");
#endif
}
