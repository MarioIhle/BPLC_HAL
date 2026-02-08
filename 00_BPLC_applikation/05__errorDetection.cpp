#include "BPLC.h"

void BPLC::setupErrorDetection()
{
   this->APP_ERROR.runntimeControl.to_runnntime.setInterval(RUNNTIME);
   this->APP_ERROR.runntimeControl.to_runnntime.reset();
   this->APP_ERROR.to_communicationError.setInterval(60000);
}
void BPLC::tickErrorDetection()
{
   if(this->systemErrorManager.errorDetectionisEnabled())
   {
      //Init überwachung
      if(this->APP_APP.setup.f_bplcSetupDone == false)
      {
         this->systemErrorManager.setError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING, __FILENAME__, __LINE__);
      }

      //Runntime überwachung der Applikation   
      if(this->APP_ERROR.runntimeControl.to_runnntime.check())
      {
         this->APP_ERROR.runntimeControl.runtimeExeeded++;
      }
      if(this->APP_ERROR.runntimeControl.runtimeExeeded >= RUNTIME_ERRORS_MAX)
      {
         //this->systemErrorManager.setError(BPLC_ERROR__RUNNTIME, __FILENAME__, __LINE__);
         this->APP_ERROR.runntimeControl.runtimeExeeded = RUNTIME_ERRORS_MAX;
      }
      this->APP_ERROR.runntimeControl.to_runnntime.reset();

      //Auf gesetzte Errors prüfen
      if(!this->systemErrorManager.noErrorSet())
      {
         this->setDeviceModeInternal(APP_MODE__SAFE_STATE);
      }

      //Netzwerkverbindung überwachen
      if(this->APP_APP.settings.device.communication.observeNetworkConnection
         && (this->APP_COM.p_comNode != nullptr))
      {
         //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
         if(this->APP_COM.p_comNode->getNodeState() == COM_NODE_STATE__AVAILABLE)
         {
            this->APP_ERROR.to_communicationError.reset();
         }
         if(this->APP_ERROR.to_communicationError.check())
         {
            this->systemErrorManager.setError(BPLC_ERROR__COMMUNICATION_FAILED, __FILENAME__, __LINE__);
         }   
      }
   }
   else
   {
      this->APP_HAL.LD3_ERROR_OUT.blinkContinious(3, 50, 50);
   }
}