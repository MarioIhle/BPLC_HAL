#include "BPLC_APP.h"

void BPLC_APP::setupSafety()
{}
void BPLC_APP::tickSafety()
{  
   //Init überwachung
   if(this->APP_APP.setup.f_bplcSetupDone == false)
   {
      this->systemErrorManager.setError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING, __FILENAME__, __LINE__);
   }
   /*
   if((this->ecmForHighSpeed->getModuleErrorCount() != 0 ) || (this->ecmForSlowSpeed->getModuleErrorCount() != 0))
   {
      this->systemErrorManager.setError(ECM_ERROR__EC_CONNECTION_FAILED, __FILENAME__, __LINE__);
   }
   */
   //Auf gesetzte Errors prüfen
   if(!this->systemErrorManager.noErrorSet())
   {
      this->setDeviceModeInternal(APP_MODE__SAFE_STATE);
   }
}