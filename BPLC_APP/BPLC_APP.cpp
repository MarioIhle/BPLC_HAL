#include "BPLC_APP.h"

//Callback für Hardware Interrupt 
bool f_ISR_called;

void INT_ISR()
{
   f_ISR_called = true;
}

BPLC_APP::BPLC_APP()
{}

/**
 * @param   INT_callBack   Zeiger auf ISR  
*/
void BPLC_APP::begin()
{
   this->hal.begin(INT_ISR); 
   Serial.println("##############################");  
   Serial.println("setup MCU11");  

   this->oled.begin();

   this->deviceSettings.f_beepOnEncoderInput = false;

   this->deviceMode = APP_MODE__START;
   this->hardwareErrorCode = BPLC_ERROR__NO_ERROR;
   
   this->temp_ParameterStorage = 0;

   //Runntime error
   this->to_runnntime.setInterval(1000);
   this->to_runnntime.reset();
   


   this->DIN11_CARD[DIN11_CARD__1].begin(DIN11_CARD_1_ADDRESS);
   this->DIN11_CARD[DIN11_CARD__2].begin(DIN11_CARD_2_ADDRESS);
   this->DIN11_CARD[DIN11_CARD__3].begin(DIN11_CARD_3_ADDRESS);
   this->DIN11_CARD[DIN11_CARD__4].begin(DIN11_CARD_4_ADDRESS);   

   this->DO11_CARD[DO11_CARD__1].begin(DO11_CARD_1_ADDRESS);
   this->DO11_CARD[DO11_CARD__2].begin(DO11_CARD_2_ADDRESS);
   this->DO11_CARD[DO11_CARD__3].begin(DO11_CARD_3_ADDRESS);
   this->DO11_CARD[DO11_CARD__4].begin(DO11_CARD_4_ADDRESS); 
}

void BPLC_APP::tick()
{
   //BPLC Hardware handling
   //BPLC 
   this->hal.tick();
   this->oled.tick();
   this->handleDisplay();
   //extension Cards
   this->handleDIN11Cards();
   this->handleAIN11Cards();
   this->handleDO11Cards();
   this->handleREL11Cards();
   this->handleMOT11Cards();

   if(f_ISR_called)
   {
      this->ISR_CALLED();
      f_ISR_called = false;
   }

   //Runntime überwachung der Applikation
   if(this->to_runnntime.check())
   {
      this->setHardwareError(BPLC_ERROR__RUNNTIME);
   }
   this->to_runnntime.reset();

   if(this->hardwareErrorCode != BPLC_ERROR__NO_ERROR)
   {
      this->deviceMode = APP_MODE__SAFE_STATE;
   }

   //Hauptapplikation BPLC
   switch(this->deviceMode)
   {
      case APP_MODE__STOP:
         this->hal.LD1.blinkWithBreak(1, 500, 500);      
         this->hal.OEN.reset();    
      break;

      case APP_MODE__RUN_WITH_CONFIG_1:   
      case APP_MODE__RUN_WITH_CONFIG_2:
      case APP_MODE__RUN_WITH_CONFIG_3:               
         this->hal.LD1.blinkWithBreak(1, 2500, 2500);           
      break;

      case APP_MODE__START:             
         this->hal.LD1.set();  
         this->hal.BUZZER.blink(1, 100); 
         this->hal.OEN.set(); 
         this->deviceMode = APP_MODE__RUN_WITH_CONFIG_1;
      break;

      case APP_MODE__SAFE_STATE:
         this->hal.LD1.blinkWithBreak(1, 100, 100);     
         this->hal.LD2.blinkWithBreak((uint8_t)this->hardwareErrorCode, 500, 1500);    
         this->hal.BUZZER.blinkWithBreak(3, 100, 30000);
         this->hal.OEN.reset();          
      break;

      default:
         this->deviceMode = APP_MODE__SAFE_STATE;
      break;
   }      
   //DEBUG_OLED_DISPLAY beep
   if(this->deviceSettings.f_beepOnEncoderInput)
   {
      this->beepOnEncoderInput(); 
   }   
}

void BPLC_APP::beep(const uint8_t BEEPS, const int BEEP_INTERVAL)
{
   this->hal.BUZZER.blink(BEEPS, BEEP_INTERVAL);
}

void BPLC_APP::handleDisplay()
{  
   switch(this->oled.getActiveMenu())
   {      
      case menu_mainMenu:
         if(this->hal.ENCODER.isButtonPressed())
         {            
            this->oled.enterMenu();            
         }

         if(this->hal.ENCODER.getTurningDirection() == movement_right)
         {
            this->oled.showNextTextOfThisMenu();
         }
         else if(this->hal.ENCODER.getTurningDirection() == movement_left)
         {
            this->oled.showPrevioursTextOfThisMenu();
         }           
         this->oled.setParamValueToShow(this->deviceMode);
      break;

      case menu_deviceMode:
         editDeviceMode();
      break;

      case menu_errorCodes:
         hardwareErrorOut();
      break;

      case menu_settings:
        
      break;

      case menu_dipSwitch:
         handle_vDip();
      break;

      case menu_screenSaver: 
         if(this->hal.ENCODER.isButtonPressed() || this->hal.ENCODER.getTurningDirection() != movement_idle)
         {
            this->oled.setMenu(menu_mainMenu);
         }
      break;
   }
}

void BPLC_APP::beepOnEncoderInput()
{
   if(this->hal.ENCODER.isButtonPressed())
   {      
      this->hal.BUZZER.blink(1, 100);
   }

   if(this->hal.ENCODER.getTurningDirection() != movement_idle)
   {
      this->hal.BUZZER.blink(1, 100);
   }
}

e_APP_MODE_t BPLC_APP::getDeviceMode()
{
   return this->deviceMode;
}

void BPLC_APP::setDeviceMode(const e_APP_MODE_t MODE)
/**
 * @param   MODE   Mode der gesetzt werden soll  
*/
{
   this->deviceMode = MODE;
}

void BPLC_APP::setVDip(const e_V_DIP_t DIP_NUM, const uint8_t VALUE)
/**
 * @param   DIP_NUM     Dip Nummer 
 * @param   VALUE       Wert von 0-255
*/
{
   this->virtualDipSwitch[DIP_NUM] = VALUE;
}

int BPLC_APP::getVDip(const e_V_DIP_t DIP_NUM)
{
   return this->virtualDipSwitch[DIP_NUM];
}

void BPLC_APP::setHardwareError(const e_BPLC_ERROR_t ERROR_CODE)
{      
   //Errors nicht überschreiben
   if(this->hardwareErrorCode == BPLC_ERROR__NO_ERROR)
   {
      this->hardwareErrorCode = ERROR_CODE;
   }   
}

//Display handling
void BPLC_APP::editDeviceMode()
{
   if(this->hal.ENCODER.isButtonPressed())
   {                 
      //Enter Parameter
      if(this->oled.parameterEntered() == false)
      {           
         this->temp_ParameterStorage = this->deviceMode;
         this->oled.enterParameter();                          
      }
      else
      {
         this->deviceMode = (e_APP_MODE_t)this->temp_ParameterStorage;
         this->oled.exitParameter();
      }
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);
      }      
   }

   if(this->hal.ENCODER.getTurningDirection() == movement_right)
   {
      if(this->oled.parameterEntered())
      {
         this->temp_ParameterStorage++;
      }
      else
      {
         this->oled.showNextTextOfThisMenu();            
      }
   }
   else if(this->hal.ENCODER.getTurningDirection() == movement_left)
   {    
      if(this->oled.parameterEntered())
      {
         this->temp_ParameterStorage--;         
      }
      else
      {
         this->oled.showPrevioursTextOfThisMenu();            
      }
   }  

   //Bereichberenzung
   if(this->temp_ParameterStorage < APP_MODE__STOP)
   {
      temp_ParameterStorage = APP_MODE__STOP;
   }
   if(this->temp_ParameterStorage >= APP_MODE__COUNT)
   {
      this->temp_ParameterStorage = APP_MODE__COUNT-1;
   }

   if(this->oled.parameterEntered())
   {
      this->oled.setParamValueToShow(this->temp_ParameterStorage);
   }
   else
   {
      this->oled.setParamValueToShow(this->deviceMode);
   }   
}

void BPLC_APP::hardwareErrorOut()
{
   if(this->hal.ENCODER.isButtonPressed())
   {            
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);         
      } 
   }

   if(this->hal.ENCODER.getTurningDirection() == movement_right)
   {
      this->oled.showNextTextOfThisMenu();
   }
   else if(this->hal.ENCODER.getTurningDirection() == movement_left)
   {
      this->oled.showPrevioursTextOfThisMenu();
   } 

   this->oled.setParamValueToShow(this->hardwareErrorCode);    
}

void BPLC_APP::handle_vDip()
{  
   const bool           IS_ENCODER_BUTTON_PRESSED  = this->hal.ENCODER.isButtonPressed();
   const e_movement_t   TURNING_DIRECTION          = this->hal.ENCODER.getTurningDirection();
   const bool           PARARMETER_IS_ENTERED      = this->oled.parameterEntered();
   const e_V_DIP_t      SELECTED_DIP               = (e_V_DIP_t)this->oled.getActiveMenuTextNum();

   if(IS_ENCODER_BUTTON_PRESSED)
   {            
      //Enter Parameter
      if(PARARMETER_IS_ENTERED == false)
      {           
         this->oled.enterParameter();  
         this->temp_ParameterStorage = this->getVDip(SELECTED_DIP);               
      }
      else
      {         
         this->setVDip(SELECTED_DIP, this->temp_ParameterStorage);  //Aktiver Text == gewählter Dip
         this->oled.exitParameter();               
      }
      //Cursor on "exit"
      if(this->oled.readyToExitMenu())
      {
         this->oled.setMenu(menu_mainMenu);
      }  
   }

   if(TURNING_DIRECTION == movement_right)
   {
      if(PARARMETER_IS_ENTERED)
      {
         this->temp_ParameterStorage++;
      }
      else
      {
         this->oled.showNextTextOfThisMenu();   
      }
   }
   else if(TURNING_DIRECTION == movement_left)
   {    
      if(PARARMETER_IS_ENTERED)
      {
         this->temp_ParameterStorage--;
      }
      else
      {
         this->oled.showPrevioursTextOfThisMenu();         
      }
   }

   if(PARARMETER_IS_ENTERED)
   {
      this->oled.setParamValueToShow(this->temp_ParameterStorage);
   }
   else
   {
      this->oled.setParamValueToShow(this->getVDip(SELECTED_DIP));
   }

#ifdef DEBUG_APP_MCU11_OLED_HANDLING
for(int i = 0; i< vDIP_COUNT; i++)
{
   Serial.print(", DIP "); Serial.print(i+1); Serial.print(": "); Serial.print(this->virtualDipSwitch[i]);
}
Serial.println("");
#endif

}

//BPLC extension Cards handling
void BPLC_APP::setupHardware(const uint8_t DIN11_CARD_COUNT, const uint8_t AIN11_CARD_COUNT, const uint8_t DO11_CARD_COUNT, const uint8_t REL11_CARD_COUNT, const uint8_t MOT11_CARD_COUNT, const uint8_t FUSE11_CARD_COUNT, const uint8_t NANO11_CARD_COUNT)
{
   this->hardware.din11CardCount    = DIN11_CARD_COUNT;
   this->hardware.ain11CardCount    = AIN11_CARD_COUNT;
   this->hardware.do11CardCount     = DO11_CARD_COUNT;
   this->hardware.rel11CardCount    = REL11_CARD_COUNT;
   this->hardware.mot11CardCount    = MOT11_CARD_COUNT;
   this->hardware.fuse11CardCount   = FUSE11_CARD_COUNT;
   this->hardware.nano11CardCount   = NANO11_CARD_COUNT;
}

void BPLC_APP::mapObjectToCard(DigitalInput* P_OBJECT, e_DIN11_CARD_t CARD)
{
   this->DIN11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
}

void BPLC_APP::mapObjectToCard(AnalogInput* P_OBJECT, e_AIN11_CARD_t CARD)
{
   this->AIN11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
}

void BPLC_APP::mapObjectToCard(Output* P_OBJECT, e_DO11_CARD_t CARD)
{
   this->DO11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
}

void BPLC_APP::mapObjectToCard(Output* P_OBJECT, e_REL11_CARD_t CARD)
{
   this->REL11_CARD[CARD].mapObjectToNextFreePort(P_OBJECT);
}

void BPLC_APP::mapObjectToCard(MOTOR* P_OBJECT, e_MOT11_CARD_t CARD)
{
   this->MOT11_CARD[CARD].mapObjectToPort(P_OBJECT);
}

void BPLC_APP::ISR_CALLED()
{
   for(uint8_t CARD=0; CARD < DIN11_CARD__MAX; CARD++)
   {
      this->DIN11_CARD[CARD].somePinOfsomeDinCardChanged();
   }

   /*
   for(uint8_t CARD=0; CARD < FUSE11_CARD__MAX; CARD++)
   {
      this->FUSE11_CARD[CARD].somePinOfsomeDinCardChanged();
   }
   */
}

void BPLC_APP::handleDIN11Cards()
{
   for(uint8_t CARD=0; CARD < DIN11_CARD__MAX; CARD++)
   {
      this->DIN11_CARD[CARD].tick();
      const e_BPLC_ERROR_t ERROR = this->DIN11_CARD[CARD].getError();
   
      if(ERROR != BPLC_ERROR__NO_ERROR)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleDO11Cards()
{
   for(uint8_t CARD=0; CARD < DO11_CARD__MAX; CARD++)
   {
      this->DO11_CARD[CARD].tick();
      const e_BPLC_ERROR_t ERROR = this->DO11_CARD[CARD].getError();
   
      if(ERROR != BPLC_ERROR__NO_ERROR)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleAIN11Cards()
{
   for(uint8_t CARD=0; CARD < AIN11_CARD__MAX; CARD++)
   {
      this->AIN11_CARD[CARD].tick();
      const e_BPLC_ERROR_t ERROR = this->AIN11_CARD[CARD].getError();
   
      if(ERROR != BPLC_ERROR__NO_ERROR)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleMOT11Cards()
{
   for(uint8_t CARD=0; CARD < MOT11_CARD__MAX; CARD++)
   {
      this->MOT11_CARD[CARD].tick();
      const e_BPLC_ERROR_t ERROR = this->MOT11_CARD[CARD].getError();
   
      if(ERROR != BPLC_ERROR__NO_ERROR)
      {
         this->setHardwareError(ERROR);
      }
   }
}

void BPLC_APP::handleREL11Cards()
{
   for(uint8_t CARD=0; CARD < REL11_CARD__MAX; CARD++)
   {
      this->REL11_CARD[CARD].tick();
      const e_BPLC_ERROR_t ERROR = this->REL11_CARD[CARD].getError();
   
      if(ERROR != BPLC_ERROR__NO_ERROR)
      {
         this->setHardwareError(ERROR);
      }
   }
}