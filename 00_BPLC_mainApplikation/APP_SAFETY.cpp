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
   if(this->APP_APP.setup.f_completeSetupDone == false)
   {
      this->setSystemError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING, __FILENAME__, __LINE__);
   }
   //Runntime überwachung der Applikation   
   if(this->APP_SAFETY.runntimeControl.to_runnntime.check())
   {
      this->APP_SAFETY.runntimeControl.runtimeExeeded++;
   }

   if(this->APP_SAFETY.runntimeControl.runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      this->setSystemError(BPLC_ERROR__RUNNTIME, __FILENAME__, __LINE__);
      this->APP_SAFETY.runntimeControl.runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();

   //Auf gesetzte Errors prüfen
   if(this->thereIsAnSystemError())
   {
      this->setDeviceMode(APP_MODE__SAFE_STATE);
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

         if(FOUND_DEVICE_IS_NOT_DEFINED && possibleDevice != 60 && possibleDevice != 188)
         {
            this->printLog("I2C Device Found but not defined, Address: " + String(possibleDevice), __FILENAME__, __LINE__);
            this->setSystemError(BPLC_ERROR__UNDEFINED_I2C_DEVICE_FOUND, __FILENAME__, __LINE__);
         }
      }     
   }      
}