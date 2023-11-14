#include "BPLC_APP.h"

void BPLC_APP::setupExtensionCards()
{
   //DIN11
   Serial.println("##############################");  
   Serial.println("setup DIN11 CARDS"); 
   this->DIN11[DIN11_CARD__1].begin(DIN11_CARD_1_ADDRESS);
   this->DIN11[DIN11_CARD__2].begin(DIN11_CARD_2_ADDRESS);
   this->DIN11[DIN11_CARD__3].begin(DIN11_CARD_3_ADDRESS);
   this->DIN11[DIN11_CARD__4].begin(DIN11_CARD_4_ADDRESS);
   Serial.println("##############################");  
   Serial.println("check DIN11 CARDS for errors ");
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < DIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->DIN11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.din11CardCount);

      //Karte antwortet, ist aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setHardwareError(DIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }

   //AIN11
   this->AIN11[AIN11_CARD__1].begin(AIN11_CARD_1_ADDRESS);
   this->AIN11[AIN11_CARD__2].begin(AIN11_CARD_2_ADDRESS);
   this->AIN11[AIN11_CARD__3].begin(AIN11_CARD_3_ADDRESS);
   this->AIN11[AIN11_CARD__4].begin(AIN11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < AIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->AIN11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.ain11CardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setHardwareError(AIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }

   //DO11
   this->DO11[DO11_CARD__1].begin(DO11_CARD_1_ADDRESS);
   this->DO11[DO11_CARD__2].begin(DO11_CARD_2_ADDRESS);
   this->DO11[DO11_CARD__3].begin(DO11_CARD_3_ADDRESS);
   this->DO11[DO11_CARD__4].begin(DO11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < DO11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->DO11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.do11CardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setHardwareError(DO11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }

   //REL11
   this->REL11[REL11_CARD__1].begin(REL11_CARD_1_ADDRESS);
   this->REL11[REL11_CARD__2].begin(REL11_CARD_2_ADDRESS);
   this->REL11[REL11_CARD__3].begin(REL11_CARD_3_ADDRESS);
   this->REL11[REL11_CARD__4].begin(REL11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < REL11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->REL11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.rel11CardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setHardwareError(REL11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }

   //MOT11
   this->MOT11[MOT11_CARD__1].begin(MOT11_CARD_1_ADDRESS);
   this->MOT11[MOT11_CARD__2].begin(MOT11_CARD_2_ADDRESS);
   this->MOT11[MOT11_CARD__3].begin(MOT11_CARD_3_ADDRESS);
   this->MOT11[MOT11_CARD__4].begin(MOT11_CARD_4_ADDRESS);
   //Auf errors prüfen
   for(uint8_t CARD = 0; CARD < MOT11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->MOT11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.mot11CardCount);

      //Karte antwortrt, aber nicht definiert
      if(ERROR == BPLC_ERROR__NO_ERROR && CARD_IN_USE == false)
      {         
         this->setHardwareError(MOT11_ERROR__CARD_FOUND_BUT_NOT_DEFINED);
      }//Karte ist definiert, hat aber fehler      
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::defineHardwareSetup(const uint8_t DIN11_CARD__COUNT, const uint8_t AIN11_CARD__COUNT, const uint8_t DO11_CARD__COUNT, const uint8_t REL11_CARD__COUNT, const uint8_t MOT11_CARD__COUNT, const uint8_t FUSE11_CARD__COUNT, const uint8_t NANO11_CARD__COUNT)
{
   this->hardwareDefinition.din11CardCount    = DIN11_CARD__COUNT;
   this->hardwareDefinition.ain11CardCount    = AIN11_CARD__COUNT;
   this->hardwareDefinition.do11CardCount     = DO11_CARD__COUNT;
   this->hardwareDefinition.rel11CardCount    = REL11_CARD__COUNT;
   this->hardwareDefinition.mot11CardCount    = MOT11_CARD__COUNT;
   this->hardwareDefinition.fuse11CardCount   = FUSE11_CARD__COUNT;
   this->hardwareDefinition.nano11CardCount   = NANO11_CARD__COUNT;
}

void BPLC_APP::mapObjectToCard(DigitalInput* P_OBJECT, e_DIN11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to DIN11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.din11CardCount)
   {
      ERROR = this->DIN11[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = DIN11_ERROR__CARD_NOT_DEFINED;
   }  
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCard(AnalogInput* P_OBJECT, e_AIN11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to AIN11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.ain11CardCount)
   {
      ERROR = this->AIN11[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = AIN11_ERROR__CARD_NOT_DEFINED;
   }      
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCard(Output* P_OBJECT, e_DO11_CARD_t CARD)
{   
   Serial.println("##############################");  
   Serial.print("map object to DO11_CARD "); Serial.println((1 + (uint8_t)CARD)); 

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.do11CardCount)
   {
      ERROR = this->DO11[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = DO11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCard(Output* P_OBJECT, e_REL11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to REL11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;
   
   if(CARD < this->hardwareDefinition.rel11CardCount)
   {
      ERROR = this->REL11[CARD].mapObjectToNextFreePort(P_OBJECT);
   }
   else
   {
      ERROR = REL11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCard(MOTOR* P_OBJECT, e_MOT11_CARD_t CARD)
{
   Serial.println("##############################");  
   Serial.print("map object to MOT11_CARD "); Serial.println((1 + (uint8_t)CARD));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.mot11CardCount)
   {
      ERROR = this->MOT11[CARD].mapObjectToPort(P_OBJECT);
   }
   else
   {
      ERROR = MOT11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(DigitalInput* P_OBJECT, const e_DIN11_CARD_t CARD, const e_DIN11_PORTS_t PORT)
{   
   Serial.println("##############################");  
   Serial.print("map object to DIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.din11CardCount)
   {
      ERROR = this->DIN11[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = DIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(Output* P_OBJECT,const e_DO11_CARD_t CARD, const e_DO11_PORTS_t PORT)
{   
   Serial.println("##############################");  
   Serial.print("map object to DO11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.do11CardCount)
   {
      ERROR = this->DO11[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = DO11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(AnalogInput* P_OBJECT, const e_AIN11_CARD_t CARD, const e_AIN11_PORTS_t PORT)
{   
   Serial.println("##############################");  
   Serial.print("map object to AIN11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.ain11CardCount)
   {
      ERROR = this->AIN11[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = AIN11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::mapObjectToCardAndPort(Output* P_OBJECT, const e_REL11_CARD_t CARD, const e_REL11_PORTS_t PORT)
{
   Serial.println("##############################");  
   Serial.print("map object to REL11_CARD "); Serial.print((1 + (uint8_t)CARD)); Serial.print(" PORT: "); Serial.println((1 + (uint8_t)PORT));

   e_BPLC_ERROR_t ERROR = BPLC_ERROR__NO_ERROR;

   if(CARD < this->hardwareDefinition.rel11CardCount)
   {
      ERROR = this->REL11[CARD].mapObjectToSpecificPort(P_OBJECT, PORT);
   }
   else
   {
      ERROR = REL11_ERROR__CARD_NOT_DEFINED;
   }   
   this->setHardwareError(ERROR); 
}

void BPLC_APP::ISR_CALLED()
{
   for(uint8_t CARD=0; CARD < DIN11_CARD__MAX; CARD++)
   {
      this->DIN11[CARD].somePinOfsomeDinCardChanged();
   }

   /*
   for(uint8_t CARD=0; CARD < FUSE11_CARD__MAX; CARD++)
   {
      this->FUSE11_CARD[CARD].somePinOfsomeRELCardChanged();
   }
   */
}

void BPLC_APP::handleDIN11Cards()
{
   for(uint8_t CARD = 0; CARD < DIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->DIN11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.din11CardCount);

      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->DIN11[CARD].tick();
      }  
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleAIN11Cards()
{
   for(uint8_t CARD=0; CARD < AIN11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->AIN11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.ain11CardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->AIN11[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleDO11Cards()
{
   for(uint8_t CARD=0; CARD < DO11_CARD__MAX; CARD++)
   {      
      const e_BPLC_ERROR_t ERROR       = this->DO11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.do11CardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->DO11[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleREL11Cards()
{
   for(uint8_t CARD=0; CARD < REL11_CARD__MAX; CARD++)
   {      
      const e_BPLC_ERROR_t ERROR       = this->REL11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.rel11CardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->REL11[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleMOT11Cards()
{
   for(uint8_t CARD=0; CARD < MOT11_CARD__MAX; CARD++)
   {
      const e_BPLC_ERROR_t ERROR       = this->MOT11[CARD].getError();
      const bool           CARD_IN_USE = (bool)(CARD < this->hardwareDefinition.mot11CardCount);
   
      if(ERROR == BPLC_ERROR__NO_ERROR)
      {
         this->MOT11[CARD].tick();
      }
      else if(ERROR == MOT11_ERROR__OEN_DISABLED && this->MCU11.OEN.getValue().value == false)
      {//Kein Fehler, wenn von MCU gesteuert abgeschaltet wird
         this->MOT11[CARD].tick();
      }
      else if(ERROR != BPLC_ERROR__NO_ERROR && CARD_IN_USE)
      {
         this->setHardwareError(ERROR);
      }
   }
}
