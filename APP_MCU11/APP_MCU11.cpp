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
      this->hal.beep(3, 500);
      Serial.println("beep reauested");
   }

   if(this->hal.getEncoderDirection() == MCU_left)
   {
      this->hal.beep(1, 100);
   }
   if(this->hal.getEncoderDirection() == MCU_right)
   {
      this->hal.beep(2,100);
   }

   //Hauptapplikation MCU
   switch(this->MODE)
   {
      case APP_MODE__STOP:
      break;

      case APP_MODE__START:
      break;

      case APP_MODE__RUNNING:         
      break;

      case APP_MODE__SAFE_STATE:
      break;

      default:
         this->MODE = APP_MODE__SAFE_STATE;
      break;
   }   
}