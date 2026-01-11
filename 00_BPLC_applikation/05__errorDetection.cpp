#include "BPLC.h"

void BPLC::setupSafety()
{
   this->APP_SAFETY.runntimeControl.to_runnntime.setInterval(RUNNTIME);
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();
}
void BPLC::tickSafety()
{
   if(this->systemErrorManager.errorDetectionisEnabled())
   {
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
   else
   {
      this->APP_HAL.LD3_ERROR_OUT.blinkContinious(3, 50, 50);
   }
}