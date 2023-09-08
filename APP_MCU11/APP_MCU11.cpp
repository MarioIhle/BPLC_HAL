#include "APP_MCU11.h"

APP_MCU11::APP_MCU11()
{}

void APP_MCU11::begin(void (*INT_callBack)(void))
/**
 * @param   INT_callBack   Zeiger auf ISR  
*/
{
   this->hal.begin(INT_callBack);   
   this->oled.begin();

   this->deviceSettings.f_beepOnEncoderInput = true;

   this->deviceMode = APP_MODE__RUN_WITH_CONFIG_1;
   memset(&this->errorCode, 0, sizeof(this->errorCode));
   
   this->temp_ParameterStorage = 0;
}

void APP_MCU11::tick()
{
   this->hal.tick();
   this->oled.tick();
   this->handleDisplay();

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
         this->hal.OEN.reset(); 
         //this->hal.LD2.reset();  //Später für Fehlercode nutzen
      break;

      default:
         this->deviceMode = APP_MODE__SAFE_STATE;
      break;
   }      
   //DEBUG beep
   if(this->deviceSettings.f_beepOnEncoderInput)
   {
      this->beepOnEncoderInput(); 
   }   
}

void APP_MCU11::beep()
{
   this->hal.BUZZER.blink(1, 500);
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

         if(this->hal.ENCODER.getTurningDirection() == right)
         {
            this->oled.showNextTextOfThisMenu();
         }
         else if(this->hal.ENCODER.getTurningDirection() == left)
         {
            this->oled.showPrevioursTextOfThisMenu();
         }           
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
         
      break;

      case menu_screenSaver: 
         if(this->hal.ENCODER.isButtonPressed() || this->hal.ENCODER.getTurningDirection() != idle)
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

   if(this->hal.ENCODER.getTurningDirection() != idle)
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

void APP_MCU11::setVDip(const uint8_t DIP_NUM, const uint8_t VALUE)
/**
 * @param   DIP_NUM     Dip Nummer 
 * @param   VALUE       Wert von 0-255
*/
{
   this->virtualDipSwitch[DIP_NUM] = VALUE;
}

int APP_MCU11::getVDip(const uint8_t DIP_NUM)
{
   return this->virtualDipSwitch[DIP_NUM];
}

void APP_MCU11::editDeviceMode()
{
   if(this->hal.ENCODER.isButtonPressed())
   {                 
      //Enter Parameter
      if(this->oled.parameterEntered() == false)
      {           
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

   if(this->hal.ENCODER.getTurningDirection() == right)
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
   else if(this->hal.ENCODER.getTurningDirection() == left)
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
   if(this->temp_ParameterStorage > APP_MODE__SAFE_STATE)
   {
      this->temp_ParameterStorage >= APP_MODE__COUNT;
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
      this->errorCode[this->temp_ParameterStorage] = false;
   }

   if(this->hal.ENCODER.getTurningDirection() == right)
   {
      this->oled.showNextTextOfThisMenu();
      this->temp_ParameterStorage++;
   }
   else if(this->hal.ENCODER.getTurningDirection() == left)
   {
      this->oled.showPrevioursTextOfThisMenu();
      this->temp_ParameterStorage--;
   } 

   //Bereichberenzung
   if(this->temp_ParameterStorage < 0)
   {
      temp_ParameterStorage = 0;
   }
   else if(this->temp_ParameterStorage > 8)
   {
      this->temp_ParameterStorage = 8;
      this->oled.setParamValueToShow(this->errorCode[this->temp_ParameterStorage]);
   }
   else
   {
      this->oled.setParamValueToShow(this->errorCode[this->temp_ParameterStorage]);
   } 
}

