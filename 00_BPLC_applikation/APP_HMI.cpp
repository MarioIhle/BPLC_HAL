#include"BPLC_APP.h"

void BPLC_APP::handleDisplay()
{  
   const bool           ENCODER_BUTTON_PRESSED  = this->APP_HAL.ENCODER.buttonPressed();
   const e_MOVEMENT_t   ENCODER_DIRETION        = this->APP_HAL.ENCODER.getTurningDirection();   
   const bool           BEEP_ON_ENCODER_INPUT   = this->APP_APP.settings.device.application.f_beepOnEncoderInput;

   if((BEEP_ON_ENCODER_INPUT && (ENCODER_DIRETION != MOVEMENT__IDLE)) //bei drehen kann das beepen abgeschaltet werden
   || this->APP_HAL.ENCODER.buttonReleased())
   {
      this->beep(1, 50);
   }

   if(this->APP_HAL.oled.getError() == BPLC_ERROR__NO_ERROR && this->APP_APP.settings.device.hardware.oledAvailable == true)
   {
      this->APP_HAL.oled.tick();

      switch(this->APP_HAL.oled.getActiveMenu())
      {      
         case menu_mainMenu:
            if(ENCODER_BUTTON_PRESSED)
            {            
               this->APP_HAL.oled.enterMenu();            
            }

            if(ENCODER_DIRETION == MOVEMENT__RIGHT)
            {
               this->APP_HAL.oled.showNextTextOfThisMenu();
            }
            else if(ENCODER_DIRETION == MOVEMENT__LEFT)
            {
               this->APP_HAL.oled.showPrevioursTextOfThisMenu();
            }           
            this->APP_HAL.oled.setParamValueToShow(this->APP_APP.deviceMode);
         break;

         case menu_deviceMode:
            editDeviceMode(ENCODER_BUTTON_PRESSED, ENCODER_DIRETION);
         break;

         case menu_errorCodes:
            hardwareErrorOut(ENCODER_BUTTON_PRESSED, ENCODER_DIRETION);
         break;

         case menu_settings:
            displaySettings(ENCODER_BUTTON_PRESSED, ENCODER_DIRETION);        
         break;

         case menu_dipSwitch:
            handle_vDip(ENCODER_BUTTON_PRESSED, ENCODER_DIRETION);
         break;

         case menu_screenSaver: 
            if(ENCODER_BUTTON_PRESSED || ENCODER_DIRETION != MOVEMENT__IDLE)
            {
               this->APP_HAL.oled.setMenu(menu_mainMenu);
            }
         break;
      }
   }   
}

void BPLC_APP::setupHMI()
{
   if (this->APP_APP.settings.device.application.f_encoderInverted)
   {
      this->APP_HAL.ENCODER.invertTurningDirection();
   } 
}

//Display handling
void BPLC_APP::editDeviceMode(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
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
         this->setDeviceModeInternal((e_APP_MODE_t)this->APP_HMI.temp_ParameterStorage);
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
   if(this->APP_HMI.temp_ParameterStorage < APP_MODE__STOP)
   {
      APP_HMI.temp_ParameterStorage = APP_MODE__STOP;
   }
   if(this->APP_HMI.temp_ParameterStorage >= APP_MODE__COUNT)
   {
      this->APP_HMI.temp_ParameterStorage = APP_MODE__COUNT-1;
   }

   if(this->APP_HAL.oled.parameterEntered())
   {
      this->APP_HAL.oled.setParamValueToShow(this->APP_HMI.temp_ParameterStorage);
   }
   else
   {
      this->APP_HAL.oled.setParamValueToShow(this->APP_APP.deviceMode);
   }   
}

void BPLC_APP::hardwareErrorOut(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
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

   this->APP_HAL.oled.setParamValueToShow(ERROR_CODE);
   
   if(ENCODER_DIRETION == MOVEMENT__RIGHT)
   {
      this->APP_HAL.oled.showNextTextOfThisMenu();
   }
   else if(ENCODER_DIRETION == MOVEMENT__LEFT)
   {
      this->APP_HAL.oled.showPrevioursTextOfThisMenu();
   }    
}

void BPLC_APP::displaySettings(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
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
      this->APP_HAL.oled.setParamValueToShow(this->APP_APP.settings.device.application.f_useBuzzer);
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

void BPLC_APP::handle_vDip(const bool ENCODER_BUTTON_PRESSED, const e_MOVEMENT_t ENCODER_DIRETION)
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
      this->APP_HAL.oled.setParamValueToShow(this->APP_HMI.temp_ParameterStorage);
   }
   else
   {
      this->APP_HAL.oled.setParamValueToShow(this->getVDip(SELECTED_DIP));
   }

#ifdef DEBUG_APP_MCU11_APP_HMI
for(int i = 0; i< vDIP_COUNT; i++)
{
   Serial.print(", DIP "); Serial.print(i+1); Serial.print(": "); Serial.print(this->virtualDipSwitch[i]);
}
Serial.println("");
#endif
}