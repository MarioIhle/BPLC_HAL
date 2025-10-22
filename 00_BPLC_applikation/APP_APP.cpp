#include "BPLC_APP.h"

void bplcTask(void* taskParameter)
{
   disableCore1WDT();
   esp_task_wdt_init(2, true);                
   esp_task_wdt_add(NULL);      

   BPLC_APP* P_APP = (BPLC_APP*)taskParameter;             

  while(1)
  {
      esp_task_wdt_reset();

      if(P_APP != nullptr) 
      {
         if(P_APP->getDeviceMode() != APP_MODE__INIT)
         {
            P_APP->tick();    
         }    
      }
      else
      {
         BPLC_logPrint log;         
         log.printResetReason("p_BPLC_APP is nullptr", __FILENAME__, __LINE__);
         abort();
      }
      delay(5);
   }
}    

BPLC_APP::BPLC_APP()
{   
   memset(&this->APP_APP, 0, sizeof(this->APP_APP));
   this->APP_APP.deviceMode = APP_MODE__INIT;
}
void BPLC_APP::begin()
{   
   Serial.begin(115200);
   this->printLog("SETUP BPLC SYSTEM", __FILENAME__, __LINE__);

   //Device Parameter aus Flash laden  
   this->setupParameterFlash();
   //this->parameterFlash.clear(); //Flash Parameter löschen 
   const bool PARAMETER_NOK = (!this->loadDeviceSettings());   

   if(PARAMETER_NOK)
   {
      //Auf Kommando warten, anschließend reset durchführen
      this->parameterFlash.clear();      
      this->printLog("CLEAR FLASH!", __FILENAME__, __LINE__);      
   }   

   //Auf neue Firmwareversion prüfen
   const bool NEW_MAJOR = (this->APP_APP.settings.device.application.versionMajor != VERSION_MAJOR);
   const bool NEW_MINOR = (this->APP_APP.settings.device.application.versionMinor != VERSION_MINOR);
   
   if(NEW_MAJOR || NEW_MINOR)
   {
      this->APP_APP.settings.device.application.versionMajor = VERSION_MAJOR;
      this->APP_APP.settings.device.application.versionMinor = VERSION_MINOR;
      this->printLog("NEW FIRMWARE VERSION DETECTED!", __FILENAME__, __LINE__);
      this->saveDeviceSettings();
   }

   //Kernsystem initialisieren   
   this->setupApplication();      
   this->setupHMI();   
   this->setupControlPanel();

   if(this->APP_APP.settings.device.autoStart)
   {           
      this->setupHardware();      
      this->setupNetwork();  
      this->setupSafety();
      
      this->APP_APP.operationMode.userApplication  = APP_MODULE_STATE__ENABLED;
      this->APP_APP.operationMode.extensionCards   = APP_MODULE_STATE__ENABLED;
      this->APP_APP.operationMode.network          = APP_MODULE_STATE__ENABLED;
      this->APP_APP.operationMode.errorOut         = APP_MODULE_STATE__ENABLED;
   }
   else
   {
      this->APP_APP.operationMode.userApplication  = APP_MODULE_STATE__NOT_INITILIZED;
      this->APP_APP.operationMode.extensionCards   = APP_MODULE_STATE__NOT_INITILIZED;
      this->APP_APP.operationMode.network          = APP_MODULE_STATE__NOT_INITILIZED;
      this->APP_APP.operationMode.errorOut         = APP_MODULE_STATE__NOT_INITILIZED;
   }

   //Fehlerprüfung bevor System startet
   this->APP_APP.setup.f_bplcSetupDone = true;

   if(this->systemErrorManager.noErrorSet())
   {
      this->printLog("BPLC SYSTEM INIT SUCCESSFUL", __FILENAME__, __LINE__);
      this->setDeviceModeInternal(APP_MODE__RUN);
   }
   else
   {
      this->printLog("BPLC SYSTEM INIT FAILED", __FILENAME__, __LINE__);
      this->setDeviceModeInternal(APP_MODE__SAFE_STATE);
   }      
}
void BPLC_APP::setupApplication()
{   
   BPLC_APP* P_APP = this;
   xTaskCreatePinnedToCore(bplcTask, "bplcTask", 4096, P_APP, 1, nullptr, 1);
   Wire.begin();   
}
void BPLC_APP::tick()
{   
   this->tickControlPanel();        
   this->handleDisplay();          
     
   //BPLC MAIN STATEMACHINE
   const bool SAFETY_ENABLED = this->APP_APP.operationMode.errorOut;
   if(SAFETY_ENABLED)
   {
      this->tickSafety();
   }

   this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 500, 500);    

   if(APP_MODULE_STATE__DISABLED == this->APP_APP.operationMode.userApplication)
   {
      this->setDeviceModeInternal(APP_MODE__STOP);
   }

   switch(this->APP_APP.deviceMode)
   {
      case APP_MODE__STOP:           
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 1000, 1000);  
         this->APP_HAL.OEN.reset(); 
         this->tickSafety();   
         this->tickHardware();    
         this->tickNetwork();             
         break;

      case APP_MODE__START: 
         this->APP_HAL.OEN.reset();        
         this->begin();         
         break;

      case APP_MODE__RUN:             
         this->APP_HAL.OEN.set();           
         this->APP_HAL.LD1_DEVICE_STATE.fade(2500, 2500);  
         this->tickSafety();
         this->tickHardware();    
         this->tickNetwork();      
         break;

      case APP_MODE__SAFE_STATE:
         this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 100, 100);     
         this->APP_HAL.LD3_ERROR_OUT.blinkContinious((uint8_t)this->systemErrorManager.getError()->errorCode , 500, 1500);    
         this->APP_HAL.BUZZER.blinkContinious(3, 100, 30000);
         this->APP_HAL.OEN.reset();   
         this->tickHardware(); 
         this->tickSafety(); 
         this->tickNetwork();        
         break;

      default:
         this->setDeviceModeInternal(APP_MODE__SAFE_STATE);
         break;
   }    
}

bool BPLC_APP::runApplikation()
{
   return (APP_MODULE_STATE__ENABLED == this->APP_APP.operationMode.userApplication);
}

//DeviceMode
void BPLC_APP::setDeviceMode(const e_APP_MODE_t MODE)
{
   if(this->getDeviceMode() != APP_MODE__SAFE_STATE)
   {
      this->setDeviceModeInternal(MODE);
   }
   else
   {
      BPLC_logPrint log;
      log.printLog("APPLICATION CAN NOT EDIT DEVICE MODE, DEVICE IS IN SAFEMODE", __FILENAME__, __LINE__);
   }
}
e_APP_MODE_t BPLC_APP::getDeviceMode()
{
   return this->APP_APP.deviceMode;
}
void BPLC_APP::setDeviceModeInternal(const e_APP_MODE_t MODE)
{
   if(this->APP_APP.deviceMode != MODE)
   {
      //Alle LEDs und Buzzer zurück setzten
      this->APP_HAL.BUZZER.reset();
      this->APP_HAL.LD1_DEVICE_STATE.reset();
      this->APP_HAL.LD2_COMMUNICATION_STATE.reset();
      this->APP_HAL.LD3_ERROR_OUT.reset();
      this->APP_HAL.OEN.reset();

      this->APP_APP.deviceMode = MODE;

      switch(MODE)
      {
         case APP_MODE__STOP:
            this->printLog("DEVICEMODE: STOP", __FILENAME__, __LINE__);
            break; 
         case APP_MODE__START:
            this->printLog("DEVICEMODE: START", __FILENAME__, __LINE__);
            break;
         case APP_MODE__SAFE_STATE:
            this->printLog("DEVICEMODE: SAFE STATE", __FILENAME__, __LINE__);
            break;
         case APP_MODE__RUN:
            this->printLog("DEVICEMODE: RUN ", __FILENAME__, __LINE__);
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