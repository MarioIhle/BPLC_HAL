#include "APP_MCU11.h"

APP_MCU11::APP_MCU11()
{}

void APP_MCU11::begin()
{
   this->hal.begin();   
   this->oled.begin();
}

void APP_MCU11::tick()
{
   this->hal.tick();
   this->oled.tick();

   //Hauptapplikation MCU
   switch(this->MODE)
   {
      case APP_MODE__STOP:
         this->hal.LD1.setupBlink(1, 500);
      break;

      case APP_MODE__START:   
         this->hal.LD1.setupBlink(1, 50);     
      break;

      case APP_MODE__RUNNING: 
         this->hal.LD1.setupBlink(1, 2500);        
      break;

      case APP_MODE__SAFE_STATE:
         this->hal.LD1.setupBlink(1, 250);
      break;

      default:
         this->MODE = APP_MODE__SAFE_STATE;
      break;
   }   
   
}



void APP_MCU11::displayStateMachine()
{  
   switch(this->oled.getActiveMenu)
   {      
      case menu_mainMenu:
         if(this->hal.isEncoderButtonPressed())
         {
            this->oled.enterMenu();
         }

         if(this->hal.getEncoderDirection() == MCU_left)
         {
            this->oled.nextText;
         }
         else if(this->hal.getEncoderDirection() == MCU_right)
         {
            this->oled.previoursText;
         }           
      break;

      case menu_deviceMode:
         if(this->hal.isEncoderButtonPressed())
         {
            this->MODE = (e_APP_MODE_t)this->oled.getSelectedDeviceMode();
         }
         if(this->hal.getEncoderDirection() == MCU_left)
         {
           this->oled.shownDeviceMode++;
         }
         else if(this->hal.getEncoderDirection() == MCU_right)
         {
           this->oled.shownDeviceMode--;
         }         
      break;

      case menu_errorCodes:
         if(this->hal.isEncoderButtonPressed())
         {
            this->oled.enterMenu();
         }

         if(this->hal.getEncoderDirection() == MCU_left)
         {
            this->oled.nextText;
         }
         else if(this->hal.getEncoderDirection() == MCU_right)
         {
            this->oled.previoursText;
         }
      break;

      case menu_settings:
         if(this->hal.isEncoderButtonPressed())
         {
            this->oled.enterMenu();
         }

         if(this->hal.getEncoderDirection() == MCU_left)
         {
            this->oled.nextText;
         }
         else if(this->hal.getEncoderDirection() == MCU_right)
         {
            this->oled.previoursText;
         }
      break;

      case menu_dipSwitch:
         if(this->hal.isEncoderButtonPressed())
         {
            this->oled.enterMenu();
         }

         if(this->hal.getEncoderDirection() == MCU_left)
         {
            this->oled.nextText;
         }
         else if(this->hal.getEncoderDirection() == MCU_right)
         {
            this->oled.previoursText;
         }
      break;
   }
}