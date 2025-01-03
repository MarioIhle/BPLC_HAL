#include "BPLC_APP.h"

void BPLC_APP::setupControlPanel()
{

}
void BPLC_APP::tickControlPanel()
{
    if(this->hostPc.commandAvailable())
    {      
        //Beep on Event
        this->beep(1, 50);        

        const u_BPLC_PLI_COMMAND_t COMMAND = this->hostPc.getCommand();

        switch (COMMAND.command.key)
        {
            //System
            case BPLC_PLI_KEY__GET_SYSTEM_ERRORS:
                this->printLog("LIST ALL SYSTEM ERRORS:", __FILENAME__, __LINE__);
                for(uint8_t error = 0; error < this->systemErrorManager.getErrorCount(); error++)
                {                    
                    this->printErrorSet(this->systemErrorManager.getError(error)->errorCode, __FILENAME__, __LINE__); 
                }
                break;
            case BPLC_PLI_KEY__GET_RAM_USAGE:
                this->printRamUsage();
                break;
                
            case BPLC_PLI_KEY__RESET_ALL_ERRORS:               
                this->systemErrorManager.resetAllErrors(__FILENAME__, __LINE__);                 
                break;

            case BPLC_PLI_KEY__GET_DEVICE_SETTINGS:
                Serial.println("HARDWARE SETTINGS: ");
                
                Serial.print("MCU:          "); Serial.println(this->APP_APP.settings.device.hardware.mcuCard);
                Serial.print("oledAvailable:"); Serial.println(this->APP_APP.settings.device.hardware.oledAvailable);              

                for(uint8_t card = 0; card < AIN11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.ain11revACards[card])
                    {
                        Serial.print("AIN11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }                    
                }                          
                for(uint8_t card = 0; card < DIN11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.din11revACards[card])
                    {
                        Serial.print("DIN11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }
                } 
                for(uint8_t card = 0; card < DO11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.do11revACards[card])
                    {
                        Serial.print("DO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < REL11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.rel11revACards[card])
                    {
                        Serial.print("REL11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < MOT11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.mot11revAcards[card])
                    {
                        Serial.print("MOT11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < TMP11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.tmp11revACards[card])
                    {
                        Serial.print("TMP11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < PPO11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.ppo11revACards[card])
                    {
                        Serial.print("PPO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                for(uint8_t card = 0; card < NANO11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.nano11revACards[card])
                    {
                        Serial.print("NANO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                for(uint8_t card = 0; card < FUSE12_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.hardware.fuse12revACards[card])
                    {
                        Serial.print("FUSE12revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                Serial.println("APPLIKATION SETTINGS:");
                Serial.print("encoder beep: ");         Serial.println(this->APP_APP.settings.device.application.f_beepOnEncoderInput);
                Serial.print("encoder inverted:  ");    Serial.println(this->APP_APP.settings.device.application.f_encoderInverted);
                Serial.print("buzzer used:  ");         Serial.println(this->APP_APP.settings.device.application.f_useBuzzer);
                Serial.println("COMMUNICATION SETTINGS:");
                Serial.print("Device adddress: ");      Serial.println(this->APP_APP.settings.device.communication.deviceAddress);
                break;

            case BPLC_PLI_KEY__RESET_ALL_SETTINGS:
                this->parameterFlash.clear();
                this->setupParameterFlash();
                Serial.println("RESET DEVICE TO CLEAR FLASH");
                ESP.restart();
                break;

            case BPLC_PLI_KEY__SOFT_RESET:
                ESP.restart();
                break;

            case BPLC_PLI_KEY__INVERT_ENCODER:
                this->APP_APP.settings.device.application.f_encoderInverted = !this->APP_APP.settings.device.application.f_encoderInverted;
                this->saveDeviceSettings();

                this->APP_HAL.ENCODER.invertTurningDirection();
                Serial.println("ENCODER INVERTETD!");
                break;

            case BPLC_PLI_KEY__USE_BUZZER:
                this->APP_APP.settings.device.application.f_useBuzzer = !this->APP_APP.settings.device.application.f_useBuzzer;
                this->saveDeviceSettings();

                if(this->APP_APP.settings.device.application.f_useBuzzer)                
                {
                    Serial.println("BUZZER IS USED!");
                    this->APP_HAL.BUZZER.setOnValue(150);
                }
                else
                {
                    Serial.println("BUZZER NOT USED!");
                    this->APP_HAL.BUZZER.setOnValue(0);
                }
                break;

            case BPLC_PLI_KEY__BEEP_ON_ENCODER_INPUT:
                this->APP_APP.settings.device.application.f_beepOnEncoderInput = !this->APP_APP.settings.device.application.f_beepOnEncoderInput;
                this->saveDeviceSettings();

                if(this->APP_APP.settings.device.application.f_beepOnEncoderInput)                
                {
                    Serial.println("BEEP ON ENCODER INPUT!");
                }
                else
                {
                    Serial.println("DONT BEEP ON ENCODER INPUT");
                }
                break;


            //Applikation
            case BPLC_PLI_KEY__DEVICE_MODE_STOP:
                this->setDeviceMode(APP_MODE__STOP);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN:
                this->setDeviceMode(APP_MODE__RUN);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_SAFETY:
                this->setDeviceMode(APP_MODE__RUN_WITHOUT_SAFETY);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_HARDWARE:
                this->setDeviceMode(APP_MODE__RUN_WITHOUT_EC_CARDS);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_COMMUNICATION:
                this->setDeviceMode(APP_MODE__RUN_WITHOUT_COM);
                break;

            
            //Kommunikation
            case BPLC_PLI_KEY__SET_DEVICE_ADDRESS:
                this->APP_APP.settings.device.communication.deviceAddress = COMMAND.command.paramValue;
                this->saveDeviceSettings();
                Serial.print("DEVICE ADDRESS SET TO "); Serial.println(this->APP_APP.settings.device.communication.deviceAddress);
                this->setupNetwork();
                break;


            //Hardware                           
            case BPLC_PLI_KEY__DEFINE_MCU:
                switch (COMMAND.command.paramValue)
                {
                    case 0:
                        this->APP_APP.settings.device.hardware.mcuCard = EC__MCU11revA;                
                        break;
                    case 1:
                        this->APP_APP.settings.device.hardware.mcuCard = EC__MCU11revB;                
                        break;
                    case 2:
                        this->APP_APP.settings.device.hardware.mcuCard = EC__MCU11revC;                
                        break;
                    
                    default:
                        break;
                }
                this->saveDeviceSettings();
                this->setupHardware();       
                break;     

            case BPLC_PLI_KEY__ADD_EC_AIN11revA:                
                this->APP_APP.settings.device.hardware.ain11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();             
                break;

            case BPLC_PLI_KEY__ADD_EC_DIN11revA:
                this->APP_APP.settings.device.hardware.din11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();             
                break;
            
            case BPLC_PLI_KEY__ADD_EC_DO11revA:
                this->APP_APP.settings.device.hardware.do11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();             
                break;       

            case BPLC_PLI_KEY__ADD_EC_REL11revA:
                this->APP_APP.settings.device.hardware.rel11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_MOT111revA:
                this->APP_APP.settings.device.hardware.mot11revAcards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_TMP11revA:
                this->APP_APP.settings.device.hardware.tmp11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_PPO11revA:
                this->APP_APP.settings.device.hardware.ppo11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_NANOrevA:
                this->APP_APP.settings.device.hardware.nano11revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_FUSE12revA:
                this->APP_APP.settings.device.hardware.fuse12revACards[COMMAND.command.paramValue] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;        

            default:
                this->printLog("host command not executable!", __FILENAME__, __LINE__);
                break;
        }        
    }
}