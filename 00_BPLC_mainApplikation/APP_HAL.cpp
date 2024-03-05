#include "BPLC_APP.h"

void BPLC_APP::invertEncoder()
{
   this->printLog("onboard Encoder invertetd");
   this->APP_HAL.ENCODER.invertTurningDirection();
}
void BPLC_APP::setupHardware()
{
   if(this->APP_APP.device.settings.mcuCard != BPLC_CARD__NO_CARD_DEFINED)
   {      
      //HAL initialisieren
      const bool MCU_HAL_INIT_OK = this->extensionCardHandler.addNewExtensionCard(this->APP_APP.device.settings.mcuCard);
      if(MCU_HAL_INIT_OK)
      {
         //BUZZER lautstärke anpassen
         this->APP_HAL.BUZZER.setOnValue(50);
         this->APP_HAL.OEN.setOnValue(1);
         //MCU IO´s mappen
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.BUZZER,                      this->APP_APP.device.settings.mcuCard, MCU_CHANNEL__BUZZER);  
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.ENCODER,                     this->APP_APP.device.settings.mcuCard, MCU_CHANNEL__ENCODER); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.OEN,                         this->APP_APP.device.settings.mcuCard, MCU_CHANNEL__OEN); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD1_DEVICE_STATE,            this->APP_APP.device.settings.mcuCard, MCU_CHANNEL__LD1); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD2_COMMUNICATION_STATE,     this->APP_APP.device.settings.mcuCard, MCU_CHANNEL__LD2); 
         this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD3_ERROR_OUT,               this->APP_APP.device.settings.mcuCard, MCU_CHANNEL__LD3);  
         //OnboardDisplay initialisieren
         this->APP_HAL.oled.begin();     
         this->APP_APP.device.settings.oledAvailable = true;
      }
   }
   else
   {
      this->setSystemError(BPLC_ERROR__NO_MCU_DEFINED);
   }
   //AIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < AIN11_CARD_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.device.settings.ain11revACards[CARD_ADDR])
      {
         switch (CARD_ADDR)
         {
            case 0:
               this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__AIN11revA_1);
               break;

            case 1:
               this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__AIN11revA_2);
               break;

            case 2:
               this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__AIN11revA_3);
               break;

            case 3:
               this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__AIN11revA_4);
               break;
            
            default:
               break;
         }   
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