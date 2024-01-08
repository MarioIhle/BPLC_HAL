#include "BPLC_APP.h"

void BPLC_APP::setHardwareError(const e_BPLC_ERROR_t ERROR_CODE)
{        
   for(uint8_t ERROR_CODE_BUFFER_SLOT = 0; ERROR_CODE_BUFFER_SLOT < HARDWARE_ERROR_BUFFER_SIZE; ERROR_CODE_BUFFER_SLOT++)
   {
      if(this->hardwareErrorCode[ERROR_CODE_BUFFER_SLOT] == ERROR_CODE)
      {//error schon gespeichert
         //Serial.println("error schon gespeichert");
         break;
      }//freier slot
      else if(this->hardwareErrorCode[ERROR_CODE_BUFFER_SLOT] == BPLC_ERROR__NO_ERROR)
      {
         Serial.print("ERROR CODE: "); Serial.print(ERROR_CODE); Serial.print(", "); Serial.println(this->errorOut.getErrorCodeText(ERROR_CODE));
         this->hardwareErrorCode[ERROR_CODE_BUFFER_SLOT] = ERROR_CODE;
         break;
      }
      else
      {//Slot schon belegt
         //Serial.println("error slot schon belegt");
      }
   }  
}

bool BPLC_APP::isThereAnyHardwareError()
{
   bool THERE_IS_AN_ERROR = false;

   for(uint8_t ERROR_CODE_BUFFER_SLOT = 0; ERROR_CODE_BUFFER_SLOT < HARDWARE_ERROR_BUFFER_SIZE; ERROR_CODE_BUFFER_SLOT++)
   {
      if(this->hardwareErrorCode[ERROR_CODE_BUFFER_SLOT] != BPLC_ERROR__NO_ERROR)
      {
         THERE_IS_AN_ERROR = true;
      }
   }
   return THERE_IS_AN_ERROR;
}
