#include "BPLC_APP.h"

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
e_BPLC_ERROR_t BPLC_APP::getSystemErrorCode(const uint8_t ERROR_CODE_SLOT)
{   
   return this->APP_SAFETY.errorCode[ERROR_CODE_SLOT];
}
void BPLC_APP::setSystemError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{        
   for(uint8_t ERROR_CODE_BUFFER_SLOT = 0; ERROR_CODE_BUFFER_SLOT < HARDWARE_ERROR_BUFFER_SIZE; ERROR_CODE_BUFFER_SLOT++)
   {
      if(this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] == ERROR_CODE)
      {
         //error schon gespeichert
         break;
      }
      else if(this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] == BPLC_ERROR__NO_ERROR)
      {     
         //freier slot
         this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] = ERROR_CODE;
         //Ausgabe des Errors auf USB Schnittstelle
         this->printError(ERROR_CODE, FILE, LINE);      
         break;
      }
      else
      {
         //Slot schon belegt        
      }
   }  
}