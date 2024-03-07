#include "BPLC_APP.h"

BPLC_APP::BPLC_APP()
{   
   memset(&this->APP_APP, 0, sizeof(this->APP_APP));
}
void BPLC_APP::begin()
{   
   Serial.begin(115200);
   this->printLog("SETUP BPLC SYSTEM", __FILENAME__, __LINE__);
   //Device Parameter aus Flash laden
   this->setupParameterFlash();
   this->readDeviceSettings();
   //Module initialisieren
   this->setupApplication();      
   this->setupHMI();   
   this->setupHardware();      
   this->setupNetwork();
   this->setupSafety();   
   
   //Fehlerprüfung bevor System startet
   if(this->getSystemErrorCode() == BPLC_ERROR__NO_ERROR)
   {
      this->printLog("BPLC SYSTEM INIT SUCCESSFUL", __FILENAME__, __LINE__);
      this->setDeviceMode(APP_MODE__START);
   }
   else
   {
      this->printLog("BPLC SYSTEM INIT FAILED", __FILENAME__, __LINE__);
      this->setDeviceMode(APP_MODE__SAFE_STATE);
   }   
   this->APP_APP.setup.f_completeSetupDone = true;
}
void BPLC_APP::setupApplication()
{   
   Wire.begin();
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

      case APP_MODE__START: 
         this->APP_HAL.OEN.reset(); 
         this->setDeviceMode(APP_MODE__RUN);
         break;

      case APP_MODE__RUN:             
         this->APP_HAL.OEN.set();           
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 2500, 2500);  
         this->tickSafety();
         this->tickHardware();    
         this->tickNetwork();      
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

      case APP_MODE__SAFE_STATE:
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 100, 100);     
         this->APP_HAL.LD3_ERROR_OUT.blinkContinious((uint8_t)getSystemErrorCode(), 500, 1500);    
         this->APP_HAL.BUZZER.blinkContinious(3, 100, 30000);
         this->APP_HAL.OEN.reset();   
         this->tickHardware(); 
         this->tickSafety(); 
         this->tickNetwork();        
         break;

      default:
         this->setDeviceMode(APP_MODE__SAFE_STATE);
         break;
   }    
}
//DeviceMode
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
      BPLC_logPrint logPrint;
      switch(MODE)
      {
         case APP_MODE__STOP:
            logPrint.printLog("DEVICEMODE: STOP", __FILENAME__, __LINE__);
            break; 
         case APP_MODE__START:
            logPrint.printLog("DEVICEMODE: START", __FILENAME__, __LINE__);
            break;
         case APP_MODE__SAFE_STATE:
            logPrint.printLog("DEVICEMODE: SAFE STATE", __FILENAME__, __LINE__);
            break;
         case APP_MODE__RUN:
            logPrint.printLog("DEVICEMODE: RUN ", __FILENAME__, __LINE__);
            break;
         case APP_MODE__RUN_WITHOUT_SAFETY:
            logPrint.printLog("DEVICEMODE: RUN CONFIG WITOUT SAFETY", __FILENAME__, __LINE__);
            break;
         case APP_MODE__RUN_WITHOUT_EC_CARDS:
            logPrint.printLog("DEVICEMODE: RUN WITHOUT EC CARDS", __FILENAME__, __LINE__);
            break;
         case APP_MODE__RUN_WITHOUT_COM:
            logPrint.printLog("DEVICEMODE: RUN WITHOUT COM", __FILENAME__, __LINE__);
            break;
      }      
   }   
}
//Dip handling
void BPLC_APP::setVDip(const e_V_DIP_t DIP_NUM, const int16_t VALUE)
{
   this->APP_APP.virtualDipSwitch[DIP_NUM] = VALUE;
}
int16_t BPLC_APP::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->APP_APP.virtualDipSwitch[DIP_NUM];
}