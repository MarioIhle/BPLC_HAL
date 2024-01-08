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
   if(this->APP_SAFETY.to_runnntime.check())
   {
      this->APP_SAFETY.runtimeExeeded++;
   }

   if(this->APP_SAFETY.runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      this->setSystemError(BPLC_ERROR__RUNNTIME);
      this->APP_SAFETY.runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->APP_SAFETY.to_runnntime.reset();

   //Auf gesetzte Errors prüfen
   if(this->therIsAnSystemError())
   {
      this->deviceMode = APP_MODE__SAFE_STATE;
   }
}

void BPLC_APP::setSystemError(const e_BPLC_ERROR_t ERROR_CODE)
{   
   uint8_t P_ERROR_CODE = (uint8_t)ERROR_CODE;
   const bool ERROR_CODE_STORED = this->APP_SAFETY.errorCodes.createNewElement(&P_ERROR_CODE, sizeof(ERROR_CODE));

   if(ERROR_CODE_STORED)
   {
      Serial.print("ERROR CODE: "); Serial.print(ERROR_CODE); Serial.print(", "); Serial.println(this->APP_SAFETY.errorOut.getErrorCodeText(ERROR_CODE));
   }   
}

bool BPLC_APP::therIsAnSystemError()
{
   return this->APP_SAFETY.errorCodes.isThereSomethingInTheBuffer();
}

e_BPLC_ERROR_t BPLC_APP::getSystemErrorCode()
{
   uint8_t ERROR_CODE_BUFFER[4];
   this->APP_SAFETY.errorCodes.getNextElement(&ERROR_CODE_BUFFER[0], sizeof(e_BPLC_ERROR_t));

   return (e_BPLC_ERROR_t)ERROR_CODE_BUFFER[0];
}
