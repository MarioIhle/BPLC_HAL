#include "BPLC_APP.h"

BPLC_extensionCardManager* p_ECM_slow;
void ecmTaskSlow(void* arg)
{
   disableCore0WDT();
   esp_task_wdt_init(1, true);                
   esp_task_wdt_add(NULL);   

   while(true)
   {
      esp_task_wdt_reset();
      p_ECM_slow->tick();
      delay(5);
   }
}
BPLC_extensionCardManager* p_ECM_fast;
void ecmTaskFast(void* arg)
{
   disableCore0WDT();
   esp_task_wdt_init(1, true);                
   esp_task_wdt_add(NULL);   

   while(true)
   {
      esp_task_wdt_reset();
      p_ECM_fast->tick();
   }
}

void BPLC_APP::setupHardware()
{  
   this->ecmForSlowSpeed = new BPLC_extensionCardManager();
   p_ECM_slow = this->ecmForSlowSpeed;    
   xTaskCreatePinnedToCore(ecmTaskSlow, "ecmTaskSlow", 4096, NULL, 1, NULL, 0);

   const e_EC_TYPE_t MCU_TYPE = this->APP_APP.settings.device.hardware.mcuCard;  

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
         this->APP_APP.settings.device.hardware.oledAvailable = true;
      }
   }
   else
   {
      this->systemErrorManager.setError(BPLC_ERROR__NO_MCU_DEFINED, __FILENAME__, __LINE__);
   }
   //Zuerst die Output Karten initialisieren
   //REL11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < REL11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.rel11revACards[CARD_ADDR])
      {
         this->ecmForSlowSpeed->addNewExtensionCard(EC__REL11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }   
   //DO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < DO11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.do11revACards[CARD_ADDR])
      {        
         this->ecmForSlowSpeed->addNewExtensionCard(EC__DO11revA, (e_EC_ADDR_t)CARD_ADDR);           
      }
   }   
   //MOT11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < MOT11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.mot11revAcards[CARD_ADDR])
      {
         this->ecmForSlowSpeed->addNewExtensionCard(EC__MOT11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //PPO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < PPO11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.ppo11revACards[CARD_ADDR])
      {
         this->ecmForSlowSpeed->addNewExtensionCard(EC__PPO11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //NANO11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < NANO11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.nano11revACards[CARD_ADDR])
      {
         this->ecmForSlowSpeed->addNewExtensionCard(EC__NANO11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //FUSE12revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < FUSE12_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.fuse12revACards[CARD_ADDR])
      {
         this->ecmForSlowSpeed->addNewExtensionCard(EC__FUSE12revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }
   //AIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < AIN11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.ain11revACards[CARD_ADDR])
      {  
         this->ecmForSlowSpeed->addNewExtensionCard(EC__AIN11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }   
   //DIN11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < DIN11_ADDRESS_COUNT; CARD_ADDR++)
   {
      const bool EC_CARD_DEFINED_IN_SETTINGS = this->APP_APP.settings.device.hardware.din11revACards[CARD_ADDR];
      if(EC_CARD_DEFINED_IN_SETTINGS)
      {   
         if(this->ecmForHighSpeed == nullptr)
         {
            this->ecmForHighSpeed = new BPLC_extensionCardManager();
            p_ECM_fast = this->ecmForHighSpeed;
            xTaskCreatePinnedToCore(ecmTaskFast, "ecmTaskFast", 4096, NULL, 1, NULL, 0);       
         }
         const bool EC_SUCCSEFUL_INITIALIZED = this->ecmForHighSpeed->addNewExtensionCard(EC__DIN11revA, (e_EC_ADDR_t)CARD_ADDR);  
         if(!EC_SUCCSEFUL_INITIALIZED)
         {
            this->systemErrorManager.setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
         }
      }             
   }
   //TMP11revA Cards initialisieren
   for (uint8_t CARD_ADDR = 0; CARD_ADDR < TMP11_ADDRESS_COUNT; CARD_ADDR++)
   {
      if(this->APP_APP.settings.device.hardware.tmp11revACards[CARD_ADDR])
      {
         this->ecmForSlowSpeed->addNewExtensionCard(EC__TMP11revA, (e_EC_ADDR_t)CARD_ADDR);
      }
   }   
}
void BPLC_APP::mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_EC_TYPE_t CARD, const e_EC_ADDR_t ADDR, const e_EC_CHANNEL_t CHANNEL)
{     
   //Prüfen ob Input       
   switch(CARD)
   {        
      case EC__DIN11revA:                                    
         this->ecmForHighSpeed->mapObjectToExtensionCard(P_IO_OBJECT, CARD, ADDR, CHANNEL);           
         break;

      default:
         this->ecmForSlowSpeed->mapObjectToExtensionCard(P_IO_OBJECT, CARD, ADDR, CHANNEL);
         break;
   } 
}
void BPLC_APP::tickHardware()
{  
   //I2C taktfrequenz auf 400kHz erhöhen
   if(Wire.getClock() != I2C_CLOCK_SPEED_400_KHZ)
   {
      Wire.setClock(I2C_CLOCK_SPEED_400_KHZ);
   }
}
void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   if(this->APP_APP.settings.device.application.f_useBuzzer)
   {
      this->APP_HAL.BUZZER.blinkOnce(BEEPS, BEEP_INTERVAL); 
   }   
}