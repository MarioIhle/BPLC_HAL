#include "BPLC_APP.h"

void BPLC_APP::setupParameterFlash()
{
   if(this->APP_APP.setup.f_setupParameterFlash == false)
   {
      const bool PARAMETER_PARTITION_AVAILABLE = this->parameterFlash.begin("parameterFlash");
      if(!PARAMETER_PARTITION_AVAILABLE)//Partitioin anlegen, wenn nicht vorhanden
      {
         this->printLog("FLASH PARAMETER PARTITION CREATED!", __FILENAME__, __LINE__);
         memset(this->APP_APP.settings.flashData, 0, sizeof(this->APP_APP.settings.flashData));
         this->parameterFlash.putBytes("deviceSettings", this->APP_APP.settings.flashData, sizeof(this->APP_APP.settings.flashData));
      }    
      this->APP_APP.setup.f_setupParameterFlash = true;  
   }
}
bool BPLC_APP::loadDeviceSettings()
{      
   bool parameterOk = false;
   //Device Settings aus Flash laden
   this->parameterFlash.getBytes("deviceSettings", this->APP_APP.settings.flashData, sizeof(this->APP_APP.settings.flashData));   
   //Checksumme berechenn
   uint8_t loByteToCheck            = this->parameterFlash.getUChar("flashCRCLoByte");      
   uint8_t hiByteToCheck            = this->parameterFlash.getUChar("flashCRCHiByte");     
   uint8_t calculatedLoByte         = 0;
   uint8_t calculatedHiByte  = 0;
   //Berechnete und gespeicherte Checksumme vergleichen
   this->calculateCrC16(this->APP_APP.settings.flashData, sizeof(this->APP_APP.settings.flashData), &calculatedLoByte, &calculatedHiByte);

   if((calculatedLoByte == loByteToCheck )
   && (calculatedHiByte == hiByteToCheck))
   {
      parameterOk = true;
   }
   else
   {
      this->systemErrorManager.setError(BPLC_ERROR__FLASH_PARAMETER_CORUPT, __FILENAME__, __LINE__);
   }
   return parameterOk;
}
void BPLC_APP::saveDeviceSettings()
{   
   this->parameterFlash.putBytes("deviceSettings", this->APP_APP.settings.flashData, sizeof(this->APP_APP.settings.flashData));  

   uint8_t loByteToCheck = 0;      
   uint8_t hiByteToCheck = 0;

   this->calculateCrC16(this->APP_APP.settings.flashData, sizeof(this->APP_APP.settings.flashData), &loByteToCheck, &hiByteToCheck);

   this->parameterFlash.putUChar("flashCRCLoByte", loByteToCheck);      
   this->parameterFlash.putUChar("flashCRCHiByte", hiByteToCheck);
   
   this->parameterFlash.end();
   
   this->printLog("DEVICE SETTING SAVED IN FLASH!", __FILENAME__, __LINE__);
}
void BPLC_APP::loadDefaultParameter()
{
   this->APP_APP.settings.device.application.f_useBuzzer = true;
}