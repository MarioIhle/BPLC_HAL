#include "BPLC_APP.h"

void BPLC_APP::setupSafety()
{
   this->APP_SAFETY.runntimeControl.to_runnntime.setInterval(RUNNTIME);
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();
   this->APP_SAFETY.to_scanI2Cbus.setInterval(60000);
   this->APP_SAFETY.to_scanI2Cbus.now();     //Wird nach Setup bei ersten tick ausgeführt, bevor OEN aktiv wird!
}
void BPLC_APP::tickSafety()
{
   //Init überwachung
   if(this->APP_APP.deviceSettings.f_initDone == false)
   {
      this->setSystemError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING);
   }
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
   //I2C Bus überwachung
   if(APP_SAFETY.to_scanI2Cbus.checkAndReset())
   {
      this->scanForUnkonwnI2CDevices();
   }
}
//I2C Bus überwachung
void BPLC_APP::scanForUnkonwnI2CDevices()
{    
   for(uint8_t possibleDevice = 0; possibleDevice < 255; possibleDevice++)
   {
      Wire.beginTransmission(possibleDevice);
      const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0); 
      if(DEVICE_FOUND)
      {
         const bool FOUND_DEVICE_IS_NOT_DEFINED = (bool)(this->extensionCardHandler.i2cAddressIsUsedByExtensionCard(possibleDevice) == false);

         if(FOUND_DEVICE_IS_NOT_DEFINED)
         {
            this->printLog("I2C Device Found but not defined, Addess: " + String(possibleDevice));
            this->setSystemError(BPLC_ERROR__UNDEFINED_I2C_DEVICE_FOUND);
         }
      }     
   }      
}
//Errorout
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
      {
         //error schon gespeichert
         break;
      }
      else if(this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] == BPLC_ERROR__NO_ERROR)
      {     
         //freier slot
         this->APP_SAFETY.errorCode[ERROR_CODE_BUFFER_SLOT] = ERROR_CODE;
         //Ausgabe des Errors auf USB Schnittstelle
         this->printError("SYSTEM ERROR CODE SET: " + String(ERROR_CODE) + ", " + String(this->APP_SAFETY.errorOut.getErrorCodeText(ERROR_CODE)));      
         break;
      }
      else
      {
         //Slot schon belegt        
      }
   }  
}