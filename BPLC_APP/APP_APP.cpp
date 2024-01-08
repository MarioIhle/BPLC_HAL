#include "BPLC_APP.h"

//Callback für Hardware Interrupt 
bool f_ISR_called;

void INT_ISR()
{
   f_ISR_called = true;
}

BPLC_APP::BPLC_APP()
{}

void BPLC_APP::begin()
{
   this->hal.begin(INT_ISR); 

   this->deviceSettings.f_useBuzzer = true;
   if(this->deviceSettings.f_useBuzzer == false)
   {
      this->hal.BUZZER.setOnValue(0);
   }
   
   Serial.println("##############################");  
   Serial.println("setup MCU11");  

   //Applikation
   this->deviceMode = APP_MODE__START;
   memset(&this->hardwareErrorCode[0], 0, sizeof(this->hardwareErrorCode));

   //Try to init all hardware
   this->oled.begin();
   this->setupExtensionCards(); 

   //Network setup
   this->setupNetwork();   

   //Setting
   this->deviceSettings.f_beepOnEncoderInput = false;   
   
   //Display handling
   this->temp_ParameterStorage = 0;

   //Runntime error
   this->to_runnntime.setInterval(RUNNTIME);
   this->to_runnntime.reset();
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
   //Network
   this->tickNetwork();

   if(f_ISR_called)
   {
      this->ISR_CALLED();
      f_ISR_called = false;
   }

   //Runntime überwachung der Applikation   
   if(this->to_runnntime.check())
   {
      this->runtimeExeeded++;
   }

   if(this->runtimeExeeded >= RUNTIME_ERRORS_MAX)
   {
      this->setHardwareError(BPLC_ERROR__RUNNTIME);
      this->runtimeExeeded = RUNTIME_ERRORS_MAX;
   }
   this->to_runnntime.reset();

   if(this->isThereAnyHardwareError())
   {
      this->deviceMode = APP_MODE__SAFE_STATE;
   }

   if(this->debug.deviceModeOld != this->deviceMode)
   {
      Serial.println("##############################");  
      Serial.print("DEVICE MODE: "); Serial.println(this->deviceMode);
      this->debug.deviceModeOld = this->deviceMode;
   }
   //Hauptapplikation BPLC
   switch(this->deviceMode)
   {
      case APP_MODE__STOP:
         this->hal.LD_DEVICE_STATE.blinkWithBreak(1, 500, 500);      
         this->hal.OEN.reset();    
      break;

      case APP_MODE__RUN_WITH_CONFIG_1:   
      case APP_MODE__RUN_WITH_CONFIG_2:
      case APP_MODE__RUN_WITH_CONFIG_3:               
         this->hal.LD_DEVICE_STATE.blinkWithBreak(1, 2500, 2500);           
      break;

      case APP_MODE__START:           
         this->hal.OEN.set(); 
         this->deviceMode = APP_MODE__RUN_WITH_CONFIG_1;
      break;

      case APP_MODE__SAFE_STATE:
         this->hal.LD_DEVICE_STATE.blinkWithBreak(1, 100, 100);     
         this->hal.LD_ERROR_OUT.blinkWithBreak((uint8_t)this->hardwareErrorCode[0], 500, 1500);    
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