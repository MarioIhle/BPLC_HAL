#include "APP_MCU11.h"

APP_MCU11::APP_MCU11()
{}

void APP_MCU11::begin()
{
   this->hal.begin();
}

void APP_MCU11::tick()
{
   this->hal.tick();

   //this->oled.tick();
   if(this->hal.isEncoderButtonPressed())
   {
      this->hal.BUZZER.setupBlink(3, 250);      
   }

   if(this->hal.getEncoderDirection() == MCU_left)
   {
    
   }

   if(this->hal.getEncoderDirection() == MCU_right)
   {
      
   }
   

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