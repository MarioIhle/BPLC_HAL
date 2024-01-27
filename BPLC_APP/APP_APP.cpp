#include "BPLC_APP.h"

BPLC_APP::BPLC_APP()
{
   this->APP_APP.deviceSettings.f_initDone = false;
}

void BPLC_APP::begin()
{   
   //Setting
   this->setupApplication();
   this->setupHardware(); 
   this->setupNetwork();   
   this->setupHMI();   
   this->setupSafety();   
   this->APP_APP.deviceSettings.f_initDone = true;
   Serial.println("##############################");
   Serial.println("BPLC SYSTEM INIT SUCCESSFUL");
}

void BPLC_APP::setupApplication()
{
   //Schnittstellen hier schon initialisieren, damit überhaupt eine Fehlerausgabe möglich ist
   Serial.begin(115200);
   Wire.begin();
   this->APP_APP.deviceSettings.f_beepOnEncoderInput  = false;   
   this->APP_APP.deviceMode                           = APP_MODE__START;
}

void BPLC_APP::tick()
{
   if(this->APP_APP.deviceSettings.f_initDone == false)
   {
      this->setSystemError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING);
   }
   //BPLC Hardware handling
   this->APP_HMI.oled.tick();
   this->handleDisplay();
   this->tickHardware();   
   this->tickNetwork();  
   this->tickSafety();  
  
   //BPLC Statemachine
   switch(this->APP_APP.deviceMode)
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
         this->APP_APP.deviceMode = APP_MODE__RUN_WITH_CONFIG_1;
      break;

      case APP_MODE__SAFE_STATE:
         this->APP_HAL.LD1_DEVICE_STATE.blinkWithBreak(1, 100, 100);     
         this->APP_HAL.LD3_ERROR_OUT.blinkWithBreak((uint8_t)getFirstSystemErrorCode(), 500, 1500);    
         this->APP_HAL.BUZZER.blinkWithBreak(3, 100, 30000);
         this->APP_HAL.OEN.reset();          
      break;

      default:
         this->APP_APP.deviceMode = APP_MODE__SAFE_STATE;
      break;
   }    
}

void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->APP_HAL.BUZZER.blink(BEEPS, BEEP_INTERVAL); 
}

e_APP_MODE_t BPLC_APP::getDeviceMode()
{
   return this->APP_APP.deviceMode;
}

void BPLC_APP::setDeviceMode(const e_APP_MODE_t MODE)
{
   this->APP_APP.deviceMode = MODE;
}

void BPLC_APP::setVDip(const e_V_DIP_t DIP_NUM, const int16_t VALUE)
{
   this->APP_APP.virtualDipSwitch[DIP_NUM] = VALUE;
}

int16_t BPLC_APP::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->APP_APP.virtualDipSwitch[DIP_NUM];
}