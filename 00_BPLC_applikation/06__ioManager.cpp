#include "BPLC.h"

void BPLC::setupHardware()
{  
   this->ecmForSlowSpeed = new BPLC_extensionCardManager();
   this->ecmForSlowSpeed->setSuperiorErrorHandlerForModule(&this->systemErrorManager);
   this->ecmForSlowSpeed->begin(5, "ECM_GENERAL_TASK");   
   const e_EC_TYPE_t MCU_TYPE = this->APP_APP.settings.device.mcuCard;  

   if(MCU_TYPE != EC__NO_TYPE_DEFINED)
   {      
      //HAL initialisieren
      const bool INIT_SUCCSESFUL = this->ecmForSlowSpeed->addNewExtensionCard(MCU_TYPE, EC_ADDR_1);
      if(INIT_SUCCSESFUL)
      {
         //Kleiner hack um buzzer aus zu schalten
         if(this->APP_APP.settings.device.application.f_useBuzzer)
         {
            this->APP_HAL.BUZZER.setOnValue(150);
         }
         else
         {
            this->APP_HAL.BUZZER.setOnValue(0);
         }         
         this->APP_HAL.OEN.setOnValue(1);
         //MCU IO´s mappen
         this->ecmForSlowSpeed->mapObjectToExtensionCard(&this->APP_HAL.BUZZER,                      MCU_TYPE, EC_ADDR_1, (e_EC_CHANNEL_t)MCU_CHANNEL__BUZZER);  
         this->ecmForSlowSpeed->mapObjectToExtensionCard(&this->APP_HAL.ENCODER,                     MCU_TYPE, EC_ADDR_1, (e_EC_CHANNEL_t)MCU_CHANNEL__ENCODER); 
         this->ecmForSlowSpeed->mapObjectToExtensionCard(&this->APP_HAL.OEN,                         MCU_TYPE, EC_ADDR_1, (e_EC_CHANNEL_t)MCU_CHANNEL__OEN); 
         this->ecmForSlowSpeed->mapObjectToExtensionCard(&this->APP_HAL.LD1_DEVICE_STATE,            MCU_TYPE, EC_ADDR_1, (e_EC_CHANNEL_t)MCU_CHANNEL__LD1); 
         this->ecmForSlowSpeed->mapObjectToExtensionCard(&this->APP_HAL.LD2_COMMUNICATION_STATE,     MCU_TYPE, EC_ADDR_1, (e_EC_CHANNEL_t)MCU_CHANNEL__LD2); 
         this->ecmForSlowSpeed->mapObjectToExtensionCard(&this->APP_HAL.LD3_ERROR_OUT,               MCU_TYPE, EC_ADDR_1, (e_EC_CHANNEL_t)MCU_CHANNEL__LD3);  
         //OnboardDisplay initialisieren
         this->APP_HAL.oled.begin();     
         this->APP_APP.settings.device.extensionCards.oledAvailable = (this->APP_HAL.oled.getError() == BPLC_ERROR__NO_ERROR);
      }
   }
   else
   {
      this->systemErrorManager.setError(BPLC_ERROR__NO_MCU_DEFINED, __FILENAME__, __LINE__);
   }
}
void BPLC::mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_EC_TYPE_t CARD, const e_EC_ADDR_t ADDR, const e_EC_CHANNEL_t CHANNEL)
{     
   //Prüfen ob Input       
   switch(CARD)
   {        
      case EC__DIN11revA:    
         if(this->ecmForHighSpeed == nullptr)
         {
            this->ecmForHighSpeed = new BPLC_extensionCardManager();    
            this->ecmForSlowSpeed->setSuperiorErrorHandlerForModule(&this->systemErrorManager);
            this->ecmForHighSpeed->begin(0, "ECM_DIN11_TASK");          
        
            const bool EC_SUCCESFUL_INITIALIZED = this->ecmForHighSpeed->addNewExtensionCard(CARD, ADDR);  
            if(!EC_SUCCESFUL_INITIALIZED)
            {
               this->systemErrorManager.setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
            }
            else
            {
               this->ecmForHighSpeed->mapObjectToExtensionCard(P_IO_OBJECT, CARD, ADDR, CHANNEL); 
            }
         }      
         else
         {
            this->systemErrorManager.setError(ECM_ERROR__EC_NOT_DEFINED, __FILENAME__, __LINE__);
         }                             
         break;

      default:
         if(this->ecmForSlowSpeed != nullptr)
         {
            this->ecmForSlowSpeed->mapObjectToExtensionCard(P_IO_OBJECT, CARD, ADDR, CHANNEL);
         }      
         else
         {
            this->systemErrorManager.setError(ECM_ERROR__EC_NOT_DEFINED, __FILENAME__, __LINE__);
         }
         break;
   } 
}
void BPLC::tickHardware()
{  
   //I2C taktfrequenz auf 400kHz erhöhen
   if(Wire.getClock() != I2C_CLOCK_SPEED_400_KHZ)
   {
      Wire.setClock(I2C_CLOCK_SPEED_400_KHZ);
   }
}
void BPLC::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   if(this->APP_APP.settings.device.application.f_useBuzzer)
   {
      this->APP_HAL.BUZZER.blinkOnce(BEEPS, BEEP_INTERVAL); 
   }   
}