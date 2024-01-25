#include "BPLC_APP.h"

void BPLC_APP::defineMCU(const e_MCU_CARD_TYPE_t CARD_TYPE)
{  
   this->APP_HAL.hardwareConfig.MCU_TYPE = CARD_TYPE;      
}

void BPLC_APP::addExtensionCard(const e_EXTENSION_CARD_TYPE_t CARDTYPE, const uint8_t CARD_COUNT)
{
   switch(CARDTYPE)
   {
      case EXTENSION_CARD__DIN11revA:
         this->APP_HAL.hardwareConfig.din11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__DO11revA:
         this->APP_HAL.hardwareConfig.do11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__AIN11revA:
         this->APP_HAL.hardwareConfig.ain11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__REL11revA:
         this->APP_HAL.hardwareConfig.rel11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__MOT11revA:
         this->APP_HAL.hardwareConfig.mot11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__NANO11revA:
         this->APP_HAL.hardwareConfig.nano11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__FUSE11revA: 
         this->APP_HAL.hardwareConfig.fuse11RevACardCount = CARD_COUNT;
      break;

      case EXTENSION_CARD__FUSE12revA:
         this->APP_HAL.hardwareConfig.fuse12RevACardCount = CARD_COUNT;
      break;
   }
}

void BPLC_APP::setupHardware()
{
   //MCU setup
   //BUZZER lautstärke anpassen
   this->APP_HAL.BUZZER.begin(50);
  
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
         this->setSystemError(BPLC_ERROR__NO_MCU_DEFINED);
      break;  
   }   
    
   //DIN11revA setup
   this->APP_HAL.DIN11_CARD[DIN11_CARD__1].begin(DIN11_CARD_1_ADDRESS);
   this->APP_HAL.DIN11_CARD[DIN11_CARD__2].begin(DIN11_CARD_2_ADDRESS);
   this->APP_HAL.DIN11_CARD[DIN11_CARD__3].begin(DIN11_CARD_3_ADDRESS);
   this->APP_HAL.DIN11_CARD[DIN11_CARD__4].begin(DIN11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < DIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.DIN11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.din11RevACardCount);

      //Karte antwortet, ist aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setSystemError(DIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }

   //AIN11revA
   this->APP_HAL.AIN11_CARD[AIN11_CARD__1].begin(AIN11_CARD_1_ADDRESS);
   this->APP_HAL.AIN11_CARD[AIN11_CARD__2].begin(AIN11_CARD_2_ADDRESS);
   this->APP_HAL.AIN11_CARD[AIN11_CARD__3].begin(AIN11_CARD_3_ADDRESS);
   this->APP_HAL.AIN11_CARD[AIN11_CARD__4].begin(AIN11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < AIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.AIN11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.ain11RevACardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setSystemError(AIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }

   //DO11revA
   this->APP_HAL.DO11_CARD[DO11_CARD__1].begin(DO11_CARD_1_ADDRESS);
   this->APP_HAL.DO11_CARD[DO11_CARD__2].begin(DO11_CARD_2_ADDRESS);
   this->APP_HAL.DO11_CARD[DO11_CARD__3].begin(DO11_CARD_3_ADDRESS);
   this->APP_HAL.DO11_CARD[DO11_CARD__4].begin(DO11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < DO11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.DO11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.do11RevACardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setSystemError(DO11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }

   //REL11revA
   this->APP_HAL.REL11_CARD[REL11_CARD__1].begin(REL11_CARD_1_ADDRESS);
   this->APP_HAL.REL11_CARD[REL11_CARD__2].begin(REL11_CARD_2_ADDRESS);
   this->APP_HAL.REL11_CARD[REL11_CARD__3].begin(REL11_CARD_3_ADDRESS);
   this->APP_HAL.REL11_CARD[REL11_CARD__4].begin(REL11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < REL11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.REL11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.rel11RevACardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setSystemError(REL11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }

   //MOT11revA
   this->APP_HAL.MOT11_CARD[MOT11_CARD__1].begin(MOT11_CARD_1_ADDRESS);
   this->APP_HAL.MOT11_CARD[MOT11_CARD__2].begin(MOT11_CARD_2_ADDRESS);
   this->APP_HAL.MOT11_CARD[MOT11_CARD__3].begin(MOT11_CARD_3_ADDRESS);
   this->APP_HAL.MOT11_CARD[MOT11_CARD__4].begin(MOT11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < MOT11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.MOT11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.mot11RevACardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setSystemError(MOT11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }
}

void BPLC_APP::mapObjectToExtensionCard(DigitalInput* P_OBJECT, const e_DIN11_CARD_t CARD, const e_DIN11_CHANNEL_t CHANNEL)
{   
   Serial.println("##############################");  
   Serial.print("map DigitalInput to DIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" CHANNEL: "); Serial.println((1 + (uint8_t)CHANNEL));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.din11RevACardCount)
   {
      ERROR = this->APP_HAL.DIN11_CARD[CARD].mapObjectToChannel(P_OBJECT, CHANNEL);
   }
   else
   {
      ERROR = DIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToExtensionCard(rpmSensor* P_OBJECT, const e_DIN11_CARD_t  CARD, const e_DIN11_CHANNEL_t CHANNEL)  
{   
   Serial.println("##############################");  
   Serial.print("map rpmSensor to DIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" CHANNEL: "); Serial.println((1 + (uint8_t)CHANNEL));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.din11RevACardCount)
   {
      ERROR = this->APP_HAL.DIN11_CARD[CARD].mapObjectToChannel(P_OBJECT, CHANNEL);
   }
   else
   {
      ERROR = DIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToExtensionCard(Output* P_OBJECT,const e_DO11_CARD_t CARD, const e_DO11_CHANNEL_t CHANNEL)
{   
   Serial.println("##############################");  
   Serial.print("map Output to DO11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" CHANNEL: "); Serial.println((1 + (uint8_t)CHANNEL));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.do11RevACardCount)
   {
      ERROR = this->APP_HAL.DO11_CARD[CARD].mapObjectToChannel(P_OBJECT, CHANNEL);
   }
   else
   {
      ERROR = DO11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToExtensionCard(servoMotor* P_OBJECT, const e_DO11_CARD_t CARD, const e_DO11_CHANNEL_t CHANNEL)
{   
   Serial.println("##############################");  
   Serial.print("map servoMotor to DO11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" CHANNEL: "); Serial.println((1 + (uint8_t)CHANNEL));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.do11RevACardCount)
   {
      ERROR = this->APP_HAL.DO11_CARD[CARD].mapObjectToChannel(P_OBJECT, CHANNEL);
   }
   else
   {
      ERROR = DO11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToExtensionCard(AnalogInput* P_OBJECT, const e_AIN11_CARD_t CARD, const e_AIN11_CHANNEL_t CHANNEL)
{   
   Serial.println("##############################");  
   Serial.print("map AnalogInput to AIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" CHANNEL: "); Serial.println((1 + (uint8_t)CHANNEL));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.ain11RevACardCount)
   {
      ERROR = this->APP_HAL.AIN11_CARD[CARD].mapObjectToChannel(P_OBJECT, CHANNEL);
   }
   else
   {
      ERROR = AIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToExtensionCard(Output* P_OBJECT, const e_REL11_CARD_t CARD, const e_REL11_PORTS_t CHANNEL)
{
   Serial.println("##############################");  
   Serial.print("map Output to REL11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" CHANNEL: "); Serial.println((1 + (uint8_t)CHANNEL));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.rel11RevACardCount)
   {
      ERROR = this->APP_HAL.REL11_CARD[CARD].mapObjectToChannel(P_OBJECT, CHANNEL);
   }
   else
   {
      ERROR = REL11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::tickHardware()
{
   this->handleMCUCard();
   this->handleDIN11Cards();
   this->handleAIN11Cards();
   this->handleDO11Cards();
   this->handleREL11Cards();
   this->handleMOT11Cards();   
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

void BPLC_APP::handleDIN11Cards()
{
   if(this->APP_HAL.INT_count > 0)
   {   
      for(uint8_t CARD = 0; CARD < DIN11_CARD__MAX; CARD++)
      {
         const e_BPLC_ERROR_t ERROR       = this->APP_HAL.DIN11_CARD[CARD].getError();
         const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.din11RevACardCount);

         if(ERROR == BPLC_ERROR__NO_ERROR)
         {
            this->APP_HAL.DIN11_CARD[CARD].tick();
         }  
         else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
         {
            this->setSystemError(ERROR);
         }
      }
      this->APP_HAL.INT_count--;
   }   
}

void BPLC_APP::handleAIN11Cards()
{
   for(uint8_t CARD=0; CARD < AIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.AIN11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.ain11RevACardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->APP_HAL.AIN11_CARD[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }
}

void BPLC_APP::handleDO11Cards()
{
   for(uint8_t CARD=0; CARD < DO11_CARD__MAX; CARD++)
   {      
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.DO11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.do11RevACardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->APP_HAL.DO11_CARD[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }
}

void BPLC_APP::handleREL11Cards()
{
   for(uint8_t CARD=0; CARD < REL11_CARD__MAX; CARD++)
   {      
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.REL11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.rel11RevACardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->APP_HAL.REL11_CARD[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }
}

void BPLC_APP::handleMOT11Cards()
{
   for(uint8_t CARD=0; CARD < MOT11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->APP_HAL.MOT11_CARD[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->APP_HAL.hardwareConfig.mot11RevACardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->APP_HAL.MOT11_CARD[CARD].tick();
      }
      else if(ERROR == MOT11_ERROR__OEN_DISABLED && this->APP_HAL.OEN.halCallback().value == false)
      {//Kein Fehler, wenn von MCU gesteuert abgeschaltet wird
         this->APP_HAL.MOT11_CARD[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }
}

