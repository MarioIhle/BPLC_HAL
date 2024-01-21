#include "BPLC_APP.h"

void BPLC_APP::setupSafety()
{
   this->APP_SAFETY.runntimeControl.to_runnntime.setInterval(RUNNTIME);
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();
}

void BPLC_APP::tickSafety()
{
   //Runntime überwachung der Applikation   
   if(this->APP_SAFETY.runntimeControl.to_runnntime.check())
   {
      this->APP_SAFETY.runntimeControl.runtimeExeeded++;
   }

   if(this->APP_SAFETY.runntimeControl.runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      this->setSystemError(BPLC_ERROR__RUNNTIME);
      this->APP_SAFETY.runntimeControl.runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();

   //Auf gesetzte Errors prüfen
   if(this->thereIsAnSystemError())
   {
      this->APP_APP.deviceMode = APP_MODE__SAFE_STATE;
   }
}

bool BPLC_APP::thereIsAnSystemError()
{
   bool THERE_IS_AN_ERROR = false;

   for(uint8_t ERROR_CODE_BUFFER_SLOT = 0; ERROR_CODE_BUFFER_SLOT < HARDWARE_ERROR_BUFFER_SIZE; ERROR_CODE_BUFFER_SLOT++)
   {
      if(this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] != BPLC_ERROR__NO_ERROR)
      {
         THERE_IS_AN_ERROR = true;
         break;
      }
   }
   return THERE_IS_AN_ERROR;
}

e_BPLC_ERROR_t BPLC_APP::getFirstSystemErrorCode()
{   
   return this->APP_SAFETY.errorCode[0];
}

void BPLC_APP::setSystemError(const e_BPLC_ERROR_t ERROR_CODE)
{        
   for(uint8_t ERROR_CODE_BUFFER_SLOT = 0; ERROR_CODE_BUFFER_SLOT < HARDWARE_ERROR_BUFFER_SIZE; ERROR_CODE_BUFFER_SLOT++)
   {
      if(this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] == ERROR_CODE)
      {//error schon gespeichert
         //Serial.println("error schon gespeichert");
         break;
      }//freier slot
      else if(this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] == BPLC_ERROR__NO_ERROR)
      {     
         this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] = ERROR_CODE;
         //Ausgabe des Errors auf USB Schnittstelle
         Serial.println("##############################");  
         Serial.println("SYSTEM ERROR OCCOURED");
         Serial.print("ERROR CODE: "); Serial.print(ERROR_CODE); Serial.print(", "); Serial.println(this->APP_SAFETY.errorOut.getErrorCodeText(ERROR_CODE));
         break;
      }
      else
      {//Slot schon belegt
         //Serial.println("error slot schon belegt");
      }
   }  
}