#include "BPLC.h"

void BPLC_APP::setupSafety()
{
   this->APP_SAFETY.runntimeControl.to_runnntime.setInterval(RUNNTIME);
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();
   this->APP_SAFETY.to_scanI2Cbus.setInterval(60000);
   this->APP_SAFETY.to_scanI2Cbus.now();     //Wird nach Setup bei ersten tick ausgeführt, bevor OEN aktiv wird!               
   
   //SystemErrorHandler an Untergeordnete Module weitergeben für Callback 
   //this->extensionCardManager.setSuperiorErrorManager(&this->systemErrorManager);   //Gibts pointer an Hal objekte weiter
}
void BPLC_APP::tickSafety()
{
   //Watchdog
   
   //Init überwachung
   if(this->APP_APP.setup.f_bplcSetupDone == false)
   {
      this->systemErrorManager.setError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING, __FILENAME__, __LINE__);
   }

   //Runntime überwachung der Applikation   
   if(this->APP_SAFETY.runntimeControl.to_runnntime.check())
   {
      this->APP_SAFETY.runntimeControl.runtimeExeeded++;
   }
   if(this->APP_SAFETY.runntimeControl.runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      //this->systemErrorManager.setError(BPLC_ERROR__RUNNTIME, __FILENAME__, __LINE__);
      this->APP_SAFETY.runntimeControl.runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();

   //Auf gesetzte Errors prüfen
   if(!this->systemErrorManager.noErrorSet())
   {
      this->setDeviceModeInternal(APP_MODE__SAFE_STATE);
   }
}