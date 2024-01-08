#include "BPLC_APP.h"

BPLC_APP::BPLC_APP()
{}

void BPLC_APP::begin()
{   
   //Applikation
   this->deviceMode = APP_MODE__START;
   memset(&this->hardwareErrorCode[0], 0, sizeof(this->hardwareErrorCode));

   //Try to init all hardware
   this->oled.begin();
   this->setupHardware(); 

   //Network setup
   this->setupNetwork();   

   //Setting
   this->deviceSettings.f_beepOnEncoderInput = false;   
   
   //Display handling
   this->temp_ParameterStorage = 0;

   //Runntime error
   this->setupSafety();   
}

void BPLC_APP::tick()
{
   //BPLC Hardware handling
   //BPLC 
   this->oled.tick();
   this->handleDisplay();
   this->tickHardware();   
   this->tickNetwork();  
   this->tickSafety();  
  
   //BPLC Statemachine
   switch(this->deviceMode)
   {
      case APP_MODE__STOP:
         this->APP_HAL.LD1_DEVICE_STATE.blinkWithBreak(1, 500, 500);      
         this->APP_HAL.OEN.reset();    
      break;

      case APP_MODE__RUN_WITH_CONFIG_1:   
      case APP_MODE__RUN_WITH_CONFIG_2:
      case APP_MODE__RUN_WITH_CONFIG_3:               
         this->APP_HAL.LD1_DEVICE_STATE.blinkWithBreak(1, 2500, 2500);           
      break;

      case APP_MODE__START:           
         this->APP_HAL.OEN.set(); 
         this->deviceMode = APP_MODE__RUN_WITH_CONFIG_1;
      break;

      case APP_MODE__SAFE_STATE:
         this->APP_HAL.LD1_DEVICE_STATE.blinkWithBreak(1, 100, 100);     
         this->APP_HAL.LD3_ERROR_OUT.blinkWithBreak((uint8_t)this->hardwareErrorCode[0], 500, 1500);    
         this->APP_HAL.BUZZER.blinkWithBreak(3, 100, 30000);
         this->APP_HAL.OEN.reset();          
      break;

      default:
         this->deviceMode = APP_MODE__SAFE_STATE;
      break;
   }    
}

void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->APP_HAL.BUZZER.blink(BEEPS, BEEP_INTERVAL); 
}

e_APP_MODE_t BPLC_APP::getDeviceMode()
{
   return this->deviceMode;
}

void BPLC_APP::setDeviceMode(const e_APP_MODE_t MODE)
{
   this->deviceMode = MODE;
}

void BPLC_APP::setVDip(const e_V_DIP_t DIP_NUM, const uint8_t VALUE)
{
   this->virtualDipSwitch[DIP_NUM] = VALUE;
}

int BPLC_APP::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->virtualDipSwitch[DIP_NUM];
}