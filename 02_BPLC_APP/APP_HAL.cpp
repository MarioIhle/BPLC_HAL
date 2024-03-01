#include "BPLC_APP.h"

void BPLC_APP::invertEncoder()
{
   this->printLog("onboard Encoder invertetd");
   this->APP_HAL.ENCODER.invertTurningDirection();
}
void BPLC_APP::setupHardware(const e_BPLC_CARD_TYPE_t MCU_TYPE)
{
   this->controlPanelHandler.setCallbackForKey(BPLC_PLI_KEY__DEFINE_MCU11revA, &this->extensionCardHandler.addNewExtensionCard);
   //BUZZER lautstärke anpassen
   this->APP_HAL.BUZZER.setOnValue(50);
   this->APP_HAL.OEN.setOnValue(1);
   //MCU setup
   this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.BUZZER,                      MCU_TYPE, MCU_CHANNEL__BUZZER);  
   this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.ENCODER,                     MCU_TYPE, MCU_CHANNEL__ENCODER); 
   this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.OEN,                         MCU_TYPE, MCU_CHANNEL__OEN); 
   this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD1_DEVICE_STATE,            MCU_TYPE, MCU_CHANNEL__LD1); 
   this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD2_COMMUNICATION_STATE,     MCU_TYPE, MCU_CHANNEL__LD2); 
   this->extensionCardHandler.mapObjectToExtensionCard(&this->APP_HAL.LD3_ERROR_OUT,               MCU_TYPE, MCU_CHANNEL__LD3);  
   //MCU Onboard OLED DISPLAY
   this->APP_HAL.oled.begin();       
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