#include "BPLC_APP.h"

void BPLC_APP::setupSafety()
{
   this->APP_SAFETY.runntimeControl.to_runnntime.setInterval(RUNNTIME);
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();
   this->APP_SAFETY.to_scanI2Cbus.setInterval(60000);
   this->APP_SAFETY.to_scanI2Cbus.now();     //Wird nach Setup bei ersten tick ausgeführt, bevor OEN aktiv wird!
   
   //SystemErrorHandler an Untergeordnete Module weitergeben für Callback 
   this->extensionCardManager.setSuperiorErrorManager(&this->systemErrorManager);   //Gibts pointer an Hal objekte weiter
}
void BPLC_APP::tickSafety()
{
   //Init überwachung
   if(this->APP_APP.setup.f_completeSetupDone == false)
   {
      this->systemErrorManager.setError(BPLC_ERROR__BPLC_BEGIN_CALL_MISSING, __FILENAME__, __LINE__);
   }

   //Runntime überwachung der Applikation   
   if(this->APP_SAFETY.runntimeControl.to_runnntime.check())
   {
      this->APP_SAFETY.runntimeControl.runtimeExeeded++;
   }
   if(this->APP_SAFETY.runntimeControl.runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      //this->systemErrorManager.setError(BPLC_ERROR__RUNNTIME, __FILENAME__, __LINE__);
      this->APP_SAFETY.runntimeControl.runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->APP_SAFETY.runntimeControl.to_runnntime.reset();

   //Auf gesetzte Errors prüfen
   if(!this->systemErrorManager.noErrorSet())
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
   for(uint8_t i2cDeviceAddress = 0; i2cDeviceAddress < 255; i2cDeviceAddress++)
   {
      Wire.beginTransmission(i2cDeviceAddress);
      const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0); 

      //Prüfen ob gefundene I2C device Adresse als BPLC device bekannt ist
      if(DEVICE_FOUND)
      {
         //AINrevA Card Addressen auf Verfügbarkeit prüfen
         for(uint8_t CARD_ADDR = 0; CARD_ADDR < AIN11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == AIN11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.ain11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(AIN11_ERROR__UNDEFINED_AIN11_FOUND, __FILENAME__, __LINE__);
            }
         }         
         //DIN11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < DO11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == DIN11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.din11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(DIN11_ERROR__UNDEFINED_DIN11_FOUND, __FILENAME__, __LINE__);
            }             
         }
         //DO11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < DO11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == DO11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.do11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(DO11_ERROR__UNDEFINED_DO11_FOUND, __FILENAME__, __LINE__);
            }
         }
         //REL11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < REL11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == REL11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.rel11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(REL11_ERROR__UNDEFINED_REL11_FOUND, __FILENAME__, __LINE__);
            }
         }
         //MOT11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < MOT11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == MOT11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.mot11revAcards[CARD_ADDR] == false)
            {
              //this->systemErrorManager.setError(MOT11_ERROR__UNDEFINED_MOT11_FOUND, __FILENAME__, __LINE__);
            }
         }
         //TMP11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < TMP11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == TMP11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.tmp11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(TMP11_ERROR__UNDEFINED_TMP11_FOUND, __FILENAME__, __LINE__);
            }
         }
         //PPO11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < PPO11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == PPO11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.ppo11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(PPO11_ERROR__UNDEFINED_PPO11_FOUND, __FILENAME__, __LINE__);
            }
         }
         //NANO11revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < NANO11_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == NANO11_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.nano11revACards[CARD_ADDR] == false)
            {
               this->systemErrorManager.setError(NANO11_ERROR__UNDEFINED_NANO11_FOUND, __FILENAME__, __LINE__);
            }
         }
         //FUSE12revA Cards Addressen auf Verfügbarkeit prüfen
         for (uint8_t CARD_ADDR = 0; CARD_ADDR < FUSE12_ADDRESS_COUNT; CARD_ADDR++)
         {
            if(i2cDeviceAddress == FUSE12_I2C_ADDRESSES[CARD_ADDR] && this->APP_APP.settings.device.hardware.fuse12revACards[CARD_ADDR] == false)
            {
               //this->systemErrorManager.setError(FUSE12_ERROR__UNDEFINED_FUSE12_FOUND, __FILENAME__, __LINE__);
            }
         }      
      }     
   }      
}