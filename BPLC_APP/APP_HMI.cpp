#include"BPLC_APP.h"

void BPLC_APP::handleDisplay()
{  
   if(this->APP_HMI.oled.getError() == BPLC_ERROR__NO_ERROR)
   {
      this->APP_HMI.oled.tick();

      switch(this->APP_HMI.oled.getActiveMenu())
      {      
         case menu_mainMenu:
            if(this->APP_HAL.ENCODER.isButtonPressed())
            {            
               this->APP_HMI.oled.enterMenu();            
            }

            if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
            {
               this->APP_HMI.oled.showNextTextOfThisMenu();
            }
            else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
            {
               this->APP_HMI.oled.showPrevioursTextOfThisMenu();
            }           
            this->APP_HMI.oled.setParamValueToShow(this->APP_APP.deviceMode);
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
               this->APP_HMI.oled.setMenu(menu_mainMenu);
            }
         break;
      }
   }   
}

void BPLC_APP::setupHMI()
{
   this->APP_HMI.oled.begin();
}

//Display handling
void BPLC_APP::editDeviceMode()
{
   if(this->APP_HAL.ENCODER.isButtonPressed())
   {                 
      //Enter Parameter
      if(this->APP_HMI.oled.parameterEntered() == false)
      {           
         this->APP_HMI.temp_ParameterStorage = this->APP_APP.deviceMode;
         this->APP_HMI.oled.enterParameter();                          
      }
      else
      {
         this->APP_APP.deviceMode = (e_APP_MODE_t)this->APP_HMI.temp_ParameterStorage;
         this->APP_HMI.oled.exitParameter();
      }
      //Cursor on "exit"
      if(this->APP_HMI.oled.readyToExitMenu())
      {
         this->APP_HMI.oled.setMenu(menu_mainMenu);
      }      
   }

   if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
   {
      if(this->APP_HMI.oled.parameterEntered())
      {
         this->APP_HMI.temp_ParameterStorage++;
      }
      else
      {
         this->APP_HMI.oled.showNextTextOfThisMenu();            
      }
   }
   else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
   {    
      if(this->APP_HMI.oled.parameterEntered())
      {
         this->APP_HMI.temp_ParameterStorage--;         
      }
      else
      {
         this->APP_HMI.oled.showPrevioursTextOfThisMenu();            
      }
   }  

   //Bereichberenzung
   if(this->APP_HMI.temp_ParameterStorage < APP_MODE__STOP)
   {
      APP_HMI.temp_ParameterStorage = APP_MODE__STOP;
   }
   if(this->APP_HMI.temp_ParameterStorage >= APP_MODE__COUNT)
   {
      this->APP_HMI.temp_ParameterStorage = APP_MODE__COUNT-1;
   }

   if(this->APP_HMI.oled.parameterEntered())
   {
      this->APP_HMI.oled.setParamValueToShow(this->APP_HMI.temp_ParameterStorage);
   }
   else
   {
      this->APP_HMI.oled.setParamValueToShow(this->APP_APP.deviceMode);
   }   
}

void BPLC_APP::hardwareErrorOut()
{
   if(this->APP_HAL.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->APP_HMI.oled.readyToExitMenu())
      {
         this->APP_HMI.oled.setMenu(menu_mainMenu);         
      } 
   }

   if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
   {
      this->APP_HMI.oled.showNextTextOfThisMenu();
   }
   else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
   {
      this->APP_HMI.oled.showPrevioursTextOfThisMenu();
   }    
}

void BPLC_APP::displaySettings()
{
   if(this->APP_HAL.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->APP_HMI.oled.readyToExitMenu())
      {
         this->APP_HMI.oled.setMenu(menu_mainMenu);         
      } 
      else
      {
         this->APP_HAL.MOT11_CARD[MOT11_CARD__1].startCurrentAutotuning();
      }
   }

   if(this->APP_HAL.ENCODER.getTurningDirection() == movement_right)
   {
      this->APP_HMI.oled.showNextTextOfThisMenu();
   }
   else if(this->APP_HAL.ENCODER.getTurningDirection() == movement_left)
   {
      this->APP_HMI.oled.showPrevioursTextOfThisMenu();
   } 
}

void BPLC_APP::handle_vDip()
{  
   const bool           IS_ENCODER_BUTTON_PRESSED  = this->APP_HAL.ENCODER.isButtonPressed();
   const e_movement_t   TURNING_DIRECTION          = this->APP_HAL.ENCODER.getTurningDirection();
   const bool           PARARMETER_IS_ENTERED      = this->APP_HMI.oled.parameterEntered();
   const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->APP_HMI.oled.getActiveMenuTextNum();

   if(IS_ENCODER_BUTTON_PRESSED)
   {            
      //Enter Parameter
      if(PARARMETER_IS_ENTERED == false)
      {           
         this->APP_HMI.oled.enterParameter();  
         this->APP_HMI.temp_ParameterStorage = this->getVDip(SELECTED_DIP);               
      }
      else
      {         
         this->setVDip(SELECTED_DIP, this->APP_HMI.temp_ParameterStorage);  //Aktiver Text == gewählter Dip
         this->APP_HMI.oled.exitParameter();               
      }
      //Cursor on "exit"
      if(this->APP_HMI.oled.readyToExitMenu())
      {
         this->APP_HMI.oled.setMenu(menu_mainMenu);
      }  
   }

   if(TURNING_DIRECTION == movement_right)
   {
      if(PARARMETER_IS_ENTERED)
      {
         this->APP_HMI.temp_ParameterStorage++;
      }
      else
      {
         this->APP_HMI.oled.showNextTextOfThisMenu();   
      }
   }
   else if(TURNING_DIRECTION == movement_left)
   {    
      if(PARARMETER_IS_ENTERED)
      {
         this->APP_HMI.temp_ParameterStorage--;
      }
      else
      {
         this->APP_HMI.oled.showPrevioursTextOfThisMenu();         
      }
   }

   if(PARARMETER_IS_ENTERED)
   {
      this->APP_HMI.oled.setParamValueToShow(this->APP_HMI.temp_ParameterStorage);
   }
   else
   {
      this->APP_HMI.oled.setParamValueToShow(this->getVDip(SELECTED_DIP));
   }

#ifdef DEBUG_APP_MCU11_APP_HMI
for(int i = 0; i< vDIP_COUNT; i++)
{
   Serial.print(", DIP "); Serial.print(i+1); Serial.print(": "); Serial.print(this->virtualDipSwitch[i]);
}
Serial.println("");
#endif
}