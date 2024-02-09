#include "BPLC_APP.h"

void BPLC_APP::invertEncoder()
{
   this->printLog("onboard Encoder invertetd");
   this->APP_HAL.ENCODER.invertTurningDirection();
}
void BPLC_APP::setupHardware()
{
   //MCU setup
   //BUZZER lautstärke anpassen
   this->APP_HAL.BUZZER.setOnValue(50);
  
   switch (this->APP_HAL.hardwareConfig.MCU_TYPE)
   {
      case MCU_CARD__MCU11revA:         
         this->APP_HAL.MCU11revA_HAL.mapEncoder(&this->APP_HAL.ENCODER);
         this->APP_HAL.MCU11revA_HAL.mapBuzzer(&this->APP_HAL.BUZZER);
         this->APP_HAL.MCU11revA_HAL.mapLD1(&this->APP_HAL.LD1_DEVICE_STATE);
         this->APP_HAL.MCU11revA_HAL.mapLD2(&this->APP_HAL.LD2_COMMUNICATION_STATE);
         this->APP_HAL.MCU11revA_HAL.mapLD3(&this->APP_HAL.LD3_ERROR_OUT);
         this->APP_HAL.MCU11revA_HAL.mapOEN(&this->APP_HAL.OEN);
         this->APP_HAL.MCU11revA_HAL.mapINT(&this->APP_HAL.INT_count);
         this->APP_HAL.MCU11revA_HAL.begin();
      break;

      case MCU_CARD__MCU11revB:         
         this->APP_HAL.MCU11revB_HAL.mapEncoder(&this->APP_HAL.ENCODER);
         this->APP_HAL.MCU11revB_HAL.mapBuzzer(&this->APP_HAL.BUZZER);
         this->APP_HAL.MCU11revB_HAL.mapLD1(&this->APP_HAL.LD1_DEVICE_STATE);
         this->APP_HAL.MCU11revB_HAL.mapLD2(&this->APP_HAL.LD2_COMMUNICATION_STATE);
         this->APP_HAL.MCU11revB_HAL.mapLD3(&this->APP_HAL.LD3_ERROR_OUT);
         this->APP_HAL.MCU11revB_HAL.mapOEN(&this->APP_HAL.OEN);
         this->APP_HAL.MCU11revB_HAL.mapINT(&this->APP_HAL.INT_count);
         this->APP_HAL.MCU11revB_HAL.begin();
      break;

      default:
      case MCU_CARD__NO_MCU_DEFINED:
         printResetReason("BPLC_HAL", "setupHardware", "MCU NOT DEFINED");
         abort();
      break;  
   }   
   //MCU Onboard OLED DISPLAY
   this->APP_HAL.oled.begin();       
}
void BPLC_APP::mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_EXTENSION_CARD_TYPE_t CARD, const uint8_t CHANNEL)
{
   this->extensionCardHandler.mapObjectToExtensionCard(P_IO_OBJECT, CARD, CHANNEL);
}
void BPLC_APP::tickHardware()
{
   while (this->APP_HAL.INT_count > 0)
   {
      for(uint8_t CARD = 0; CARD < DIN11_CARD_ADDRESS_COUNT; CARD++)
      {
         //this->APP_HAL.DIN11_CARD[CARD].ISR_callback();
      }
      this->APP_HAL.INT_count--;
   }
   
   this->handleMCUCard();
   this->extensionCardHandler.tickAllextensionCards();
   this->setSystemError(this->extensionCardHandler.getError());
}
void BPLC_APP::handleMCUCard()
{  
   switch (this->APP_HAL.hardwareConfig.MCU_TYPE)
   {
      case MCU_CARD__MCU11revA:
         this->APP_HAL.MCU11revA_HAL.tick();
         break;

      case MCU_CARD__MCU11revB:
         this->APP_HAL.MCU11revB_HAL.tick();       
         break;

      default:
      case MCU_CARD__NO_MCU_DEFINED:
         this->setSystemError(BPLC_ERROR__NO_MCU_DEFINED);
         break;  
   }
}