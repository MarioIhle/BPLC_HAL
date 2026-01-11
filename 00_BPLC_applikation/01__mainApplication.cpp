#include "BPLC.h"

void bplcTask(void* taskParameter)
{
   disableCore1WDT();
   esp_task_wdt_init(2, true);                
   esp_task_wdt_add(NULL);      

   BPLC* P_APP = (BPLC*)taskParameter;             

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
         log.printResetReason("p_BPLC is nullptr", __FILENAME__, __LINE__);
         abort();
      }
      delay(5);
   }
}    

BPLC::BPLC()
{   
   memset(&this->APP_APP, 0, sizeof(this->APP_APP));
   this->APP_APP.deviceMode = APP_MODE__INIT;
}
void BPLC::begin()
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

   //Module initialisieren   
   this->setupApplication();      
   this->setupHMI();   
   this->setupHardware();      
   this->setupNetwork();  
   this->setupErrorDetection();
   this->setupControlPanel();

   if(this->APP_APP.settings.device.autoStart)
   {           
      this->setupHardware();      
      this->setupNetwork();  
      this->setupSafety();
      
      this->APP_APP.operationMode.userApplication  = APP_MODULE_STATE__ENABLED;
      this->APP_APP.operationMode.extensionCards   = APP_MODULE_STATE__ENABLED;
      this->APP_APP.operationMode.network          = APP_MODULE_STATE__ENABLED;
      this->APP_APP.operationMode.errorOut           = APP_MODULE_STATE__ENABLED;
   }
   else
   {
      this->APP_APP.operationMode.userApplication  = APP_MODULE_STATE__NOT_INITILIZED;
      this->APP_APP.operationMode.extensionCards   = APP_MODULE_STATE__NOT_INITILIZED;
      this->APP_APP.operationMode.network          = APP_MODULE_STATE__NOT_INITILIZED;
      this->APP_APP.operationMode.errorOut           = APP_MODULE_STATE__NOT_INITILIZED;
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
void BPLC::resetDeviceParameter()
{
   this->setupParameterFlash();
   this->parameterFlash.clear();
   this->printResetReason("RESET FLASH PARAMETER", __FILENAME__, __LINE__);
   ESP.restart();
}
void BPLC::setupApplication()
{   
   BPLC* P_APP = this;
   xTaskCreatePinnedToCore(bplcTask, "bplcTask", 4096, P_APP, 1, nullptr, 1);
   Wire.begin();   
   this->APP_APP.softReset.to_resetDelay.setInterval(1500, false);
}
void BPLC::tick()
{   
   this->tickControlPanel();        
   this->tickHMI();          
     
   //BPLC MAIN STATEMACHINE
   switch(this->APP_APP.deviceMode)
   {
      case APP_MODE__RUN:           
      case APP_MODE__SAFE_STATE: 
      case APP_MODE__STOP:         
         this->tickErrorDetection();   
         this->tickHardware(); 
         this->tickNetwork();
         break;

      default:
         this->setDeviceModeInternal(APP_MODE__SAFE_STATE);
         break;
   }    

   const bool SOFT_RESET_TRIGGERD = this->APP_APP.softReset.to_resetDelay.check();
   if(SOFT_RESET_TRIGGERD)
   {     
      abort();
   }
}
//Softreset
void BPLC::triggerSoftReset()
{
   if(TIMEOUT_STATE__NOT_STATED == this->APP_APP.softReset.to_resetDelay.getState())
   {
      this->printLog("SOFT RESET TRIGGERD", __FILENAME__, __LINE__);
      this->APP_HAL.LD1_DEVICE_STATE.blinkOnce(20, 50);
      this->beep(20, 50);
      this->APP_APP.softReset.to_resetDelay.reset();
   }
}

//DeviceMode
void BPLC::setDeviceMode(const e_APP_MODE_t MODE)
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
e_APP_MODE_t BPLC::getDeviceMode()
{
   return this->APP_APP.deviceMode;
}
void BPLC::setDeviceModeInternal(const e_APP_MODE_t MODE)
{
   if(this->APP_APP.deviceMode != MODE)
   {
      this->APP_APP.deviceMode = MODE;

      switch(MODE)
      {
         case APP_MODE__STOP:
            this->printLog("DEVICEMODE: STOP", __FILENAME__, __LINE__);
            this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 500, 500);      
            this->APP_HAL.OEN.reset(); 
            this->beep(1, 500);
            break; 

         case APP_MODE__SAFE_STATE:
            this->printLog("DEVICEMODE: SAFE STATE", __FILENAME__, __LINE__);
            this->APP_HAL.LD1_DEVICE_STATE.blinkContinious(1, 100, 100);     
            this->APP_HAL.LD3_ERROR_OUT.blinkContinious((uint8_t)this->systemErrorManager.getError()->errorCode , 500, 1500);    
            this->APP_HAL.BUZZER.blinkContinious(3, 100, 30000);
            this->APP_HAL.OEN.reset();  
            break;

         case APP_MODE__RUN:
            this->printLog("DEVICEMODE: RUN ", __FILENAME__, __LINE__);
            this->APP_HAL.OEN.set();           
            this->APP_HAL.LD1_DEVICE_STATE.fade(500, 500); 
            this->APP_HAL.BUZZER.blinkOnce(1, 50);
            break;
      }      
   }   
}
//Dip handling
void BPLC::setVDip(const e_V_DIP_t DIP_NUM, const int16_t VALUE)
{
   this->APP_APP.virtualDipSwitch[DIP_NUM] = VALUE;
}
int16_t BPLC::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->APP_APP.virtualDipSwitch[DIP_NUM];
}