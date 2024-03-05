#include "BPLC_APP.h"

void BPLC_APP::setupParameterFlash()
{
   if(this->APP_APP.setup.f_setupParameterFlash == false)
   {
      const bool PARTITION_AVAILABLE = this->parameterFlash.begin("deviceSettings");
      if(!PARTITION_AVAILABLE)//Partitioin anlegen, wenn nicht vorhanden
      {
         memset(this->APP_APP.device.data, 0, sizeof(this->APP_APP.device.data));
         this->parameterFlash.putBytes("deviceSettings", this->APP_APP.device.data, sizeof(this->APP_APP.device.data));
      }
      this->APP_APP.setup.f_setupParameterFlash = true;
   }
}
void BPLC_APP::readDeviceSettings()
{
   this->parameterFlash.getBytes("deviceSettings", this->APP_APP.device.data, sizeof(this->APP_APP.device.data));   
}
void BPLC_APP::saveDeviceSettings()
{
   this->parameterFlash.putBytes("deviceSettings", this->APP_APP.device.data, sizeof(this->APP_APP.device.data));  
}