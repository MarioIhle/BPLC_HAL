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
   this->oled.tick();
   

   //Hauptapplikation MCU
   switch(this->MODE)
   {
      case APP_MODE__STOP:
      break;

      case APP_MODE__START:
      break;

      case APP_MODE__RUNNING:
         this->mainLoop();
      break;

      case APP_MODE__SAFE_STATE:
      break;

      default:
         this->MODE = APP_MODE__SAFE_STATE;
      break;
   }   
}