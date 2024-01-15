#include"BPLC_APP.h"
#include "BPLC_CARDS.h"

//Callback für Hardware Interrupt 
bool f_ISR_called;

void INT_ISR()
{
   f_ISR_called = true;
}

void BPLC_APP::ISR_CALLED()
{
   for(uint8_t CARD=0; CARD < DIN11_CARD__MAX; CARD++)
   {
      this->APP_HAL.DIN11_CARD[CARD].somePinOfsomeDinCardChanged();
   }

   /*
   for(uint8_t CARD=0; CARD < FUSE11_CARD__MAX; CARD++)
   {
      this->APP_HAL.FUSE11_CARD[CARD].somePinOfsomeRELCardChanged();
   }
   */
}

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
   //Encoder
   this->APP_HAL.ENCODER.begin(&this->APP_HAL.Encoder_A, &this->APP_HAL.Encoder_B, &this->APP_HAL.Encoder_Z);
   //LD1-3
   this->APP_HAL.LD1_DEVICE_STATE.begin(255);
   this->APP_HAL.LD2_COMMUNICATION_STATE.begin(255);     
   this->APP_HAL.LD3_ERROR_OUT.begin(255);    
   //BUZZER
   this->APP_HAL.BUZZER.begin(50);
   //P_OEN
   this->APP_HAL.OEN.begin(true);  

   switch (this->APP_HAL.hardwareConfig.MCU_TYPE)
   {
      case MCU_CARD__MCU11revA:         
         this->APP_HAL.MCU11revA_HAL.mapEncoder(&this->APP_HAL.Encoder_A, &this->APP_HAL.Encoder_B, &this->APP_HAL.Encoder_Z);
         this->APP_HAL.MCU11revA_HAL.mapBuzzer(&this->APP_HAL.BUZZER);
         this->APP_HAL.MCU11revA_HAL.mapLD1(&this->APP_HAL.LD1_DEVICE_STATE);
         this->APP_HAL.MCU11revA_HAL.mapLD2(&this->APP_HAL.LD2_COMMUNICATION_STATE);
         this->APP_HAL.MCU11revA_HAL.mapLD3(&this->APP_HAL.LD3_ERROR_OUT);
         this->APP_HAL.MCU11revA_HAL.mapOEN(&this->APP_HAL.OEN);
         this->APP_HAL.MCU11revA_HAL.begin(&INT_ISR);
      break;

      case MCU_CARD__MCU11revB:         
         this->APP_HAL.MCU11revB_HAL.mapEncoder(&this->APP_HAL.Encoder_A, &this->APP_HAL.Encoder_B, &this->APP_HAL.Encoder_Z);
         this->APP_HAL.MCU11revB_HAL.mapBuzzer(&this->APP_HAL.BUZZER);
         this->APP_HAL.MCU11revB_HAL.mapLD1(&this->APP_HAL.LD1_DEVICE_STATE);
         this->APP_HAL.MCU11revB_HAL.mapLD2(&this->APP_HAL.LD2_COMMUNICATION_STATE);
         this->APP_HAL.MCU11revB_HAL.mapLD3(&this->APP_HAL.LD3_ERROR_OUT);
         this->APP_HAL.MCU11revB_HAL.mapOEN(&this->APP_HAL.OEN);
         this->APP_HAL.MCU11revB_HAL.begin(&INT_ISR);
      break;

      default:
      case MCU_CARD__NO_MCU_DEFINED:
         this->setSystemError(BPLC_ERROR__NO_MCU_DEFINED);
      break;  
   }   
    

   //DIN11revA setup
   Serial.println("##############################");  
   Serial.println("setup DIN11 CARDS"); 
   this->APP_HAL.DIN11_CARD[DIN11_CARD__1].begin(DIN11_CARD_1_ADDRESS);
   this->APP_HAL.DIN11_CARD[DIN11_CARD__2].begin(DIN11_CARD_2_ADDRESS);
   this->APP_HAL.DIN11_CARD[DIN11_CARD__3].begin(DIN11_CARD_3_ADDRESS);
   this->APP_HAL.DIN11_CARD[DIN11_CARD__4].begin(DIN11_CARD_4_ADDRESS);
   Serial.println("##############################");  
   Serial.println("check DIN11 CARDS for errors ");
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

void BPLC_APP::tickHardware()
{
   if(f_ISR_called)
   {
      this->ISR_CALLED();
      f_ISR_called = false;
   }  

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


//BPLC extension Cards handling
void BPLC_APP::mapObjectToCard(DigitalInput* P_OBJECT, e_DIN11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to DIN11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.din11RevACardCount)
   {
      ERROR = this->APP_HAL.DIN11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = DIN11_ERROR__CARD_NOT_DEFINED;
   }  
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCard(AnalogInput* P_OBJECT, e_AIN11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to AIN11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.ain11RevACardCount)
   {
      ERROR = this->APP_HAL.AIN11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = AIN11_ERROR__CARD_NOT_DEFINED;
   }      
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCard(Output* P_OBJECT, e_DO11_CARD_t CARD)
{   
   Serial.println("##############################");  
   Serial.print("map object to DO11_CARD "); Serial.println((1 + (uint8_t)CARD)); 

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.do11RevACardCount)
   {
      ERROR = this->APP_HAL.DO11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = DO11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCard(Output* P_OBJECT, e_REL11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to REL11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;
   
   if(CARD < this->APP_HAL.hardwareConfig.rel11RevACardCount)
   {
      ERROR = this->APP_HAL.REL11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = REL11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCard(MOTOR* P_OBJECT, e_MOT11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to MOT11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.mot11RevACardCount)
   {
      ERROR = this->APP_HAL.MOT11_CARD[CARD].mapObjectToPort(P_OBJECT);
   }
   else
   {
      ERROR = MOT11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(DigitalInput* P_OBJECT, const e_DIN11_CARD_t CARD, const e_DIN11_PORTS_t PORT)
{   
   Serial.println("##############################");  
   Serial.print("map object to DIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.din11RevACardCount)
   {
      ERROR = this->APP_HAL.DIN11_CARD[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = DIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(Output* P_OBJECT,const e_DO11_CARD_t CARD, const e_DO11_PORTS_t PORT)
{   
   Serial.println("##############################");  
   Serial.print("map object to DO11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.do11RevACardCount)
   {
      ERROR = this->APP_HAL.DO11_CARD[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = DO11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(AnalogInput* P_OBJECT, const e_AIN11_CARD_t CARD, const e_AIN11_PORTS_t PORT)
{   
   Serial.println("##############################");  
   Serial.print("map object to AIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.ain11RevACardCount)
   {
      ERROR = this->APP_HAL.AIN11_CARD[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = AIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(Output* P_OBJECT, const e_REL11_CARD_t CARD, const e_REL11_PORTS_t PORT)
{
   Serial.println("##############################");  
   Serial.print("map object to REL11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->APP_HAL.hardwareConfig.rel11RevACardCount)
   {
      ERROR = this->APP_HAL.REL11_CARD[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = REL11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setSystemError(ERROR); 
}

void BPLC_APP::handleDIN11Cards()
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
      else if(ERROR == MOT11_ERROR__OEN_DISABLED && this->APP_HAL.OEN.getValue().value == false)
      {//Kein Fehler, wenn von MCU gesteuert abgeschaltet wird
         this->APP_HAL.MOT11_CARD[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setSystemError(ERROR);
      }
   }
}

