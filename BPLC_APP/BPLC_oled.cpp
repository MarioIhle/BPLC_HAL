#include "BPLC_APP.h"


void BPLC_APP::handleDisplay()
{  
   switch(this->OLED.getActiveMenu())
   {      
      case menu_mainMenu:
         if(this->MCU11.ENCODER.isButtonPressed())
         {            
            this->OLED.enterMenu();            
         }

         if(this->MCU11.ENCODER.getTurningDirection() == movement_right)
         {
            this->OLED.showNextTextOfThisMenu();
         }
         else if(this->MCU11.ENCODER.getTurningDirection() == movement_left)
         {
            this->OLED.showPrevioursTextOfThisMenu();
         }           
         this->OLED.setParamValueToShow(this->deviceMode);
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
         if(this->MCU11.ENCODER.isButtonPressed() || this->MCU11.ENCODER.getTurningDirection() != movement_idle)
         {
            this->OLED.setMenu(menu_mainMenu);
         }
      break;
   }
}

void BPLC_APP::editDeviceMode()
{
   if(this->MCU11.ENCODER.isButtonPressed())
   {                 
      //Enter Parameter
      if(this->OLED.parameterEntered() == false)
      {           
         this->temp_ParameterStorage = this->deviceMode;
         this->OLED.enterParameter();                          
      }
      else
      {
         this->deviceMode = (e_APP_MODE_t)this->temp_ParameterStorage;
         this->OLED.exitParameter();
      }
      //Cursor on "exit"
      if(this->OLED.readyToExitMenu())
      {
         this->OLED.setMenu(menu_mainMenu);
      }      
   }

   if(this->MCU11.ENCODER.getTurningDirection() == movement_right)
   {
      if(this->OLED.parameterEntered())
      {
         this->temp_ParameterStorage++;
      }
      else
      {
         this->OLED.showNextTextOfThisMenu();            
      }
   }
   else if(this->MCU11.ENCODER.getTurningDirection() == movement_left)
   {    
      if(this->OLED.parameterEntered())
      {
         this->temp_ParameterStorage--;         
      }
      else
      {
         this->OLED.showPrevioursTextOfThisMenu();            
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

   if(this->OLED.parameterEntered())
   {
      this->OLED.setParamValueToShow(this->temp_ParameterStorage);
   }
   else
   {
      this->OLED.setParamValueToShow(this->deviceMode);
   }   
}

void BPLC_APP::hardwareErrorOut()
{
   if(this->MCU11.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->OLED.readyToExitMenu())
      {
         this->OLED.setMenu(menu_mainMenu);         
      } 
   }

   if(this->MCU11.ENCODER.getTurningDirection() == movement_right)
   {
      this->OLED.showNextTextOfThisMenu();
   }
   else if(this->MCU11.ENCODER.getTurningDirection() == movement_left)
   {
      this->OLED.showPrevioursTextOfThisMenu();
   } 

   this->OLED.setParamValueToShow(this->hardwareErrorCode[0]);    
}

void BPLC_APP::displaySettings()
{
   if(this->MCU11.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->OLED.readyToExitMenu())
      {
         this->OLED.setMenu(menu_mainMenu);         
      } 
      else
      {
         this->MOT11[MOT11_CARD__1].startCurrentAutotuning();
      }
   }

   if(this->MCU11.ENCODER.getTurningDirection() == movement_right)
   {
      this->OLED.showNextTextOfThisMenu();
   }
   else if(this->MCU11.ENCODER.getTurningDirection() == movement_left)
   {
      this->OLED.showPrevioursTextOfThisMenu();
   } 
}

void BPLC_APP::handle_vDip()
{  
   const bool           IS_ENCODER_BUTTON_PRESSED  = this->MCU11.ENCODER.isButtonPressed();
   const e_movement_t   TURNING_DIRECTION          = this->MCU11.ENCODER.getTurningDirection();
   const bool           PARARMETER_IS_ENTERED      = this->OLED.parameterEntered();
   const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->OLED.getActiveMenuTextNum();

   if(IS_ENCODER_BUTTON_PRESSED)
   {            
      //Enter Parameter
      if(PARARMETER_IS_ENTERED == false)
      {           
         this->OLED.enterParameter();  
         this->temp_ParameterStorage = this->getVDip(SELECTED_DIP);               
      }
      else
      {         
         this->setVDip(SELECTED_DIP, this->temp_ParameterStorage);  //Aktiver Text == gewählter Dip
         this->OLED.exitParameter();               
      }
      //Cursor on "exit"
      if(this->OLED.readyToExitMenu())
      {
         this->OLED.setMenu(menu_mainMenu);
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
         this->OLED.showNextTextOfThisMenu();   
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
         this->OLED.showPrevioursTextOfThisMenu();         
      }
   }

   if(PARARMETER_IS_ENTERED)
   {
      this->OLED.setParamValueToShow(this->temp_ParameterStorage);
   }
   else
   {
      this->OLED.setParamValueToShow(this->getVDip(SELECTED_DIP));
   }

#ifdef DEBUG_APP_MCU11_OLED_HANDLING
for(int i = 0; i< vDIP_COUNT; i++)
{
   Serial.print(", DIP "); Serial.print(i+1); Serial.print(": "); Serial.print(this->virtualDipSwitch[i]);
}
Serial.println("");
#endif

}
