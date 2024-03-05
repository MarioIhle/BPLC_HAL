#include "BPLC_APP.h"

void BPLC_APP::setupHardware()
{
   const e_BPLC_CARD_TYPE_t   MCU_TYPE          = this->APP_APP.settings.device.hardware.mcuCard;  

   if(MCU_TYPE != BPLC_CARD__NO_CARD_DEFINED)
   {      
      //HAL initialisieren
      const bool INIT_SUCCSESFUL = this->extensionCardHandler.addNewExtensionCard(MCU_TYPE, 0);
      if(INIT_SUCCSESFUL)
      {
         //BUZZER lautstärke anpassen
         this->APP_HAL.BUZZER.setOnValue(150);
         this->APP_HAL.OEN.setOnValue(1);
         //MCU IO´s mappen
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.BUZZER,                      MCU_TYPE, MCU_CHANNEL__BUZZER);  
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.ENCODER,                     MCU_TYPE, MCU_CHANNEL__ENCODER); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.OEN,                         MCU_TYPE, MCU_CHANNEL__OEN); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD1_DEVICE_STATE,            MCU_TYPE, MCU_CHANNEL__LD1); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD2_COMMUNICATION_STATE,     MCU_TYPE, MCU_CHANNEL__LD2); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD3_ERROR_OUT,               MCU_TYPE, MCU_CHANNEL__LD3);  
         //OnboardDisplay initialisieren
         this->APP_HAL.oled.begin();     
         this->APP_APP.settings.device.hardware.oledAvailable = true;
      }
   }
   else
   {
      this->setSystemError(BPLC_ERROR__NO_MCU_DEFINED);
   }

   //AIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < AIN11_CARD_I2C_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.ain11revACards[CARD_ADDR])
      {  
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__AIN11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }   
   //DIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < DIN11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.din11revACards[CARD_ADDR])
      {   
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__DIN11revA, (e_EC_ADDR_t)CARD_ADDR);  
      }             
   }
   //DO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < DO11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.do11revACards[CARD_ADDR])
      {        
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__DO11revA, (e_EC_ADDR_t)CARD_ADDR);           
      }
   }
   //REL11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < REL11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.rel11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__REL11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //MOT11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < MOT11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.mot11revAcards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MOT11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //TMP11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < TMP11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.tmp11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__TMP11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //PPO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < PPO11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.ppo11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__PPO11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //NANO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < NANO11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.nano11revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__NANO11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //FUSE11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < FUSE12_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.fuse12revACards[CARD_ADDR])
      {
         this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__FUSE11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
}
void BPLC_APP::mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const uint8_t CHANNEL)
{
   this->extensionCardHandler.mapObjectToExtensionCard(P_IO_OBJECT, CARD, CHANNEL);
}
void BPLC_APP::tickHardware()
{  
   this->extensionCardHandler.tick();
   this->setSystemError(this->extensionCardHandler.getModulError());
}
void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->APP_HAL.BUZZER.blinkOnce(BEEPS, BEEP_INTERVAL); 
}