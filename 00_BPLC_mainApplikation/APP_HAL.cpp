#include "BPLC_APP.h"

void BPLC_APP::setupHardware()
{
   const e_BPLC_CARD_TYPE_t   MCU_TYPE          = this->APP_APP.settings.device.hardware.mcuCard;  

   if(MCU_TYPE != BPLC_CARD__NO_CARD_DEFINED)
   {      
      //HAL initialisieren
      const bool INIT_SUCCSESFUL = this->extensionCardHandler.addNewExtensionCard(MCU_TYPE, EC_ADDR_1);
      if(INIT_SUCCSESFUL)
      {
         //BUZZER lautstärke anpassen
         this->APP_HAL.BUZZER.setOnValue(150);
         this->APP_HAL.OEN.setOnValue(1);
         //MCU IO´s mappen
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.BUZZER,                      MCU_TYPE, EC_ADDR_1, MCU_CHANNEL__BUZZER);  
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.ENCODER,                     MCU_TYPE, EC_ADDR_1, MCU_CHANNEL__ENCODER); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.OEN,                         MCU_TYPE, EC_ADDR_1, MCU_CHANNEL__OEN); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD1_DEVICE_STATE,            MCU_TYPE, EC_ADDR_1, MCU_CHANNEL__LD1); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD2_COMMUNICATION_STATE,     MCU_TYPE, EC_ADDR_1, MCU_CHANNEL__LD2); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD3_ERROR_OUT,               MCU_TYPE, EC_ADDR_1, MCU_CHANNEL__LD3);  
         //OnboardDisplay initialisieren
         this->APP_HAL.oled.begin();     
         this->APP_APP.settings.device.hardware.oledAvailable = true;
      }
   }
   else
   {
      this->setSystemError(BPLC_ERROR__NO_MCU_DEFINED, __FILENAME__, __LINE__);
   }

   //AIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_AIN11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.ain11revACards[CARD_ADDR])
      {  
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__AIN11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }   
   //DIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_DIN11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.din11revACards[CARD_ADDR])
      {   
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__DIN11revA, (e_EC_ADDR_t)CARD_ADDR);  
      }             
   }
   //DO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_DO11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.do11revACards[CARD_ADDR])
      {        
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__DO11revA, (e_EC_ADDR_t)CARD_ADDR);           
      }
   }
   //REL11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_REL11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.rel11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__REL11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //MOT11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_MOT11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.mot11revAcards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MOT11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //TMP11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_TMP11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.tmp11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__TMP11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //PPO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_PPO11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.ppo11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__PPO11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //NANO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_NANO11__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.nano11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__NANO11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //FUSE11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < I2C_ADDRESS_FUSE12__COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.fuse12revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__FUSE11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
}
void BPLC_APP::mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const e_EC_ADDR_t ADDR, const uint8_t CHANNEL)
{
   this->extensionCardHandler.mapObjectToExtensionCard(P_IO_OBJECT, CARD, ADDR, CHANNEL);
}
void BPLC_APP::tickHardware()
{  
   this->extensionCardHandler.tick();
   this->setSystemError(this->extensionCardHandler.getModulError(), __FILENAME__, __LINE__);
}
void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->APP_HAL.BUZZER.blinkOnce(BEEPS, BEEP_INTERVAL); 
}