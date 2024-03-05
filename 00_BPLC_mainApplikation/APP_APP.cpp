#include "BPLC_APP.h"

BPLC_APP::BPLC_APP()
{   
   this->APP_APP.deviceSettings.f_initDone = false;
}
void BPLC_APP::begin()
{   
   Serial.begin(115200);
   this->printLog("SETUP BPLC SYSTEM");
   //Schlüsselparameter übernehemen
   this->APP_COM.deviceAddress = 0;

   //Setup submodule
   this->setupApplication();      
   this->setupHMI();   
   this->setupHardware();      
   this->setupNetwork();
   this->setupSafety();   
   
   //Fehlerprüfung bevor System startet
   if(this->getFirstSystemErrorCode() == BPLC_ERROR__NO_ERROR)
   {
      this->printLog("BPLC SYSTEM INIT SUCCESSFUL");
   }
   else
   {
      this->printLog("BPLC SYSTEM INIT FAILED");
   }   
   this->APP_APP.deviceSettings.f_initDone = true;
}
void BPLC_APP::setupApplication()
{   
   Wire.begin();
   this->APP_APP.deviceSettings.f_beepOnEncoderInput = false;   

   if(APP_APP.deviceSettings.hardware.mcuCard == BPLC_CARD__NO_CARD_DEFINED)
   {
      this->APP_APP.deviceMode = APP_MODE__SAFE_STATE;
      this->APP_APP.deviceSettings.hardware.oledAvailable = false;
   }
   else
   {
      this->APP_APP.deviceMode = APP_MODE__START;
   }   
}
void BPLC_APP::tick()
{   
   this->tickControlPanel();        
   this->handleDisplay();          
     
   //BPLC MAIN STATEMACHINE
   switch(this->APP_APP.deviceMode)
   {
      case APP_MODE__STOP:
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 500, 500);      
         this->APP_HAL.OEN.reset(); 
         this->tickSafety();   
         this->tickHardware(); 
         this->tickNetwork();
         break;

      case APP_MODE__RUN:             
         this->APP_HAL.OEN.set();           
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 2500, 2500);  
         this->tickSafety();
         this->tickHardware();         
         break;

      case APP_MODE__RUN_WITHOUT_SAFETY:   
         this->APP_HAL.OEN.set();       
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 1000, 1000);     
         this->tickHardware();   
         this->tickNetwork();       
         break;

      case APP_MODE__RUN_WITHOUT_EC_CARDS:
         this->APP_HAL.OEN.reset();
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 1000, 1000);
         this->tickSafety();
         this->tickNetwork();
         break;

      case APP_MODE__RUN_WITHOUT_COM:
         this->APP_HAL.OEN.set();        
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 1000, 1000);   
         this->tickHardware();
         this->tickSafety();
         break;

      case APP_MODE__START: //OEN nicht aktiv, einmal gesamtes System durchlaufen um mögliche Fehler zu detektieren.         
         this->APP_APP.deviceMode = APP_MODE__RUN;
         break;

      case APP_MODE__SAFE_STATE:
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 100, 100);     
         this->APP_HAL.LD3_ERROR_OUT.blinkContinious((uint8_t)getFirstSystemErrorCode(), 500, 1500);    
         this->APP_HAL.BUZZER.blinkContinious(3, 100, 30000);
         this->APP_HAL.OEN.reset();   
         this->tickHardware(); 
         this->tickSafety(); 
         this->tickNetwork();        
         break;

      default:
         this->APP_APP.deviceMode = APP_MODE__SAFE_STATE;
         break;
   }    
}
void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->APP_HAL.BUZZER.blinkOnce(BEEPS, BEEP_INTERVAL); 
}
e_APP_MODE_t BPLC_APP::getDeviceMode()
{
   return this->APP_APP.deviceMode;
}
void BPLC_APP::setDeviceMode(const e_APP_MODE_t MODE)
{
   if(this->APP_APP.deviceMode != MODE)
   {
      this->APP_APP.deviceMode = MODE;
      //Log Print
      BPLC_LOG logPrint;
      switch(MODE)
      {
         case APP_MODE__STOP:
            logPrint.printLog("DEVICEMODE: STOP");
            break; 
         case APP_MODE__START:
            logPrint.printLog("DEVICEMODE: START");
            break;
         case APP_MODE__SAFE_STATE:
            logPrint.printLog("DEVICEMODE: SAFE STATE");
            break;
         case APP_MODE__RUN:
            logPrint.printLog("DEVICEMODE: RUN ");
            break;
         case APP_MODE__RUN_WITHOUT_SAFETY:
            logPrint.printLog("DEVICEMODE: RUN CONFIG WITOUT SAFETY");
            break;
         case APP_MODE__RUN_WITHOUT_EC_CARDS:
            logPrint.printLog("DEVICEMODE: RUN WITHOUT EC CARDS");
            break;
         case APP_MODE__RUN_WITHOUT_COM:
            logPrint.printLog("DEVICEMODE: RUN WITHOUT COM");
            break;
      }      
   }   
}
void BPLC_APP::setVDip(const e_V_DIP_t DIP_NUM, const int16_t VALUE)
{
   this->APP_APP.virtualDipSwitch[DIP_NUM] = VALUE;
}
int16_t BPLC_APP::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->APP_APP.virtualDipSwitch[DIP_NUM];
}