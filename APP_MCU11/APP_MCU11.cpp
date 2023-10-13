#include "APP_MCU11.h"

APP_MCU11::APP_MCU11()
{}

/**
 * @param   INT_callBack   Zeiger auf ISR  
*/
void APP_MCU11::begin(void (*INT_callBack)(void))
{
   this->hal.begin(INT_callBack); 
   Serial.println("##############################");  
   Serial.println("setup MCU11");  

   this->oled.begin();

   this->deviceSettings.f_beepOnEncoderInput = false;

   this->deviceMode = APP_MODE__RUN_WITH_CONFIG_1;
   this->hardwareErrorCode = BPLC_ERROR__NO_ERROR;
   
   this->temp_ParameterStorage = 0;

   //Runntime error
   this->to_runnntime.setInterval(1000);
   this->to_runnntime.reset();
}

void APP_MCU11::tick()
{
   //Runntime überwachung der Applikation
   if(this->to_runnntime.check())
   {
      this->setHardwareError(BPLC_ERROR__RUNNTIME);
   }
   this->to_runnntime.reset();

   if(this->isThereAnyHardwareError())
   {
      this->deviceMode = APP_MODE__SAFE_STATE;
   }

   //Hauptapplikation MCU
   switch(this->deviceMode)
   {
      case APP_MODE__STOP:
         this->hal.LD1.blink(1, 500);      
         this->hal.OEN.reset();    
      break;

      case APP_MODE__RUN_WITH_CONFIG_1:   
      case APP_MODE__RUN_WITH_CONFIG_2:
      case APP_MODE__RUN_WITH_CONFIG_3:               
         this->hal.LD1.blink(1, 2500);   
         this->hal.OEN.set();         
      break;

      case APP_MODE__START:             
         this->hal.LD1.set();   
      break;

      case APP_MODE__SAFE_STATE:
         this->hal.LD1.blink(1, 100);     
         this->hal.LD2.blinkWithBreak((uint8_t)this->hardwareErrorCode, 500, 1500);    
         this->hal.BUZZER.blinkWithBreak(3, 50, 30000);
         this->hal.OEN.reset();          
      break;

      default:
         this->deviceMode = APP_MODE__SAFE_STATE;
      break;
   }      
   //DEBUG_OLED_DISPLAY beep
   if(this->deviceSettings.f_beepOnEncoderInput)
   {
      this->beepOnEncoderInput(); 
   }   

   this->hal.tick();
   this->oled.tick();
   this->handleDisplay();
}

void APP_MCU11::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->hal.BUZZER.blink(BEEPS, BEEP_INTERVAL);
}

void APP_MCU11::handleDisplay()
{  
   switch(this->oled.getActiveMenu())
   {      
      case menu_mainMenu:
         if(this->hal.ENCODER.isButtonPressed())
         {            
            this->oled.enterMenu();            
         }

         if(this->hal.ENCODER.getTurningDirection() == movement_right)
         {
            this->oled.showNextTextOfThisMenu();
         }
         else if(this->hal.ENCODER.getTurningDirection() == movement_left)
         {
            this->oled.showPrevioursTextOfThisMenu();
         }           
         this->oled.setParamValueToShow(this->deviceMode);
      break;

      case menu_deviceMode:
         editDeviceMode();
      break;

      case menu_errorCodes:
         errorOut();
      break;

      case menu_settings:
        
      break;

      case menu_dipSwitch:
         handle_vDip();
      break;

      case menu_screenSaver: 
         if(this->hal.ENCODER.isButtonPressed() || this->hal.ENCODER.getTurningDirection() != movement_idle)
         {
            this->oled.setMenu(menu_mainMenu);
         }
      break;
   }
}

void APP_MCU11::beepOnEncoderInput()
{
   if(this->hal.ENCODER.isButtonPressed())
   {      
      this->hal.BUZZER.blink(1, 100);
   }

   if(this->hal.ENCODER.getTurningDirection() != movement_idle)
   {
      this->hal.BUZZER.blink(1, 100);
   }
}

e_APP_MODE_t APP_MCU11::getDeviceMode()
{
   return this->deviceMode;
}

void APP_MCU11::setDeviceMode(const e_APP_MODE_t MODE)
/**
 * @param   MODE   Mode der gesetzt werden soll  
*/
{
   this->deviceMode = MODE;
}

void APP_MCU11::setVDip(const e_V_DIP_t DIP_NUM, const uint8_t VALUE)
/**
 * @param   DIP_NUM     Dip Nummer 
 * @param   VALUE       Wert von 0-255
*/
{
   this->virtualDipSwitch[DIP_NUM] = VALUE;
}

int APP_MCU11::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->virtualDipSwitch[DIP_NUM];
}

void APP_MCU11::setHardwareError(const e_BPLC_ERROR_t ERROR_CODE)
{      
   //Errors nicht überschreiben
   if(this->hardwareErrorCode == BPLC_ERROR__NO_ERROR)
   {
      this->hardwareErrorCode = ERROR_CODE;
   }   
}

bool APP_MCU11::isThereAnyHardwareError()
{
   const bool THERE_IS_A_BPLC_HARDWARE_ERROR = (bool)(this->hardwareErrorCode != BPLC_ERROR__NO_ERROR);

   return THERE_IS_A_BPLC_HARDWARE_ERROR;
}

//Diplay handling
void APP_MCU11::editDeviceMode()
{
   if(this->hal.ENCODER.isButtonPressed())
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

   if(this->hal.ENCODER.getTurningDirection() == movement_right)
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
   else if(this->hal.ENCODER.getTurningDirection() == movement_left)
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

void APP_MCU11::errorOut()
{
   if(this->hal.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);         
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

   this->oled.setParamValueToShow(this->hardwareErrorCode);    
}

void APP_MCU11::handle_vDip()
{  
   const bool           IS_ENCODER_BUTTON_PRESSED  = this->hal.ENCODER.isButtonPressed();
   const e_movement_t   TURNING_DIRECTION          = this->hal.ENCODER.getTurningDirection();
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

