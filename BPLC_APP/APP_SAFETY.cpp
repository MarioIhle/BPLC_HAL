#include "BPLC_APP.h"

void BPLC_APP::setupSafety()
{
   this->APP_SAFETY.errorCodes.begin();
   this->APP_SAFETY.to_runnntime.setInterval(RUNNTIME);
   this->APP_SAFETY.to_runnntime.reset();
}

void BPLC_APP::tickSafety()
{
   //Runntime überwachung der Applikation   
   if(this->to_runnntime.check())
   {
      this->runtimeExeeded++;
   }

   if(this->runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      this->setHardwareError(BPLC_ERROR__RUNNTIME);
      this->runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->to_runnntime.reset();

   //Auf gesetzte Errors prüfen
   if(this->isThereAnyHardwareError())
   {
      this->deviceMode = APP_MODE__SAFE_STATE;
   }
}

void BPLC_APP::setSystemError(const e_BPLC_ERROR_t ERROR_CODE)
{   
   const bool ERROR_CODE_STORED = this->APP_SAFETY.errorCodes.createNewElement(&ERROR_CODE, sizeof(ERROR_CODE));

   if(ERROR_CODE_STORED)
   {
      Serial.print("ERROR CODE: "); Serial.print(ERROR_CODE); Serial.print(", "); Serial.println(this->errorOut.getErrorCodeText(ERROR_CODE));
   }   
}

bool BPLC_APP::therIsAnSystemError()
{
   return this->APP_SAFETY.errorCodes.isThereSomethingInTheBuffer();
}
