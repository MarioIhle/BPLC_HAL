#include "BPLC_APP.h"

#define hostStartFrame  36  //ASCII $
#define hostEndFrame    37  //ASCII %

typedef enum
{   
    //System
    BPLC_PLI_KEY__GET_SYSTEM_ERRORS,
    BPLC_PLI_KEY__RESET_ALL_ERRORS,
    BPLC_PLI_KEY__GET_DEVICE_SETTINGS,
    BPLC_PLI_KEY__RESET_ALL_SETTINGS,
    BPLC_PLI_KEY__RESET_EXTENSION_CARDS,
    BPLC_PLI_KEY__SOFT_RESET,
    BPLC_PLI_KEY__INVERT_ENCODER,
    BPLC_PLI_KEY__USE_BUZZER,
    BPLC_PLI_KEY__BEEP_ON_ENCODER_INPUT,
    BPLC_PLI_KEY__GET_RAM_USAGE,

    //Applikation
    BPLC_PLI_KEY__DEVICE_MODE_STOP = 0x10,
    BPLC_PLI_KEY__DEVICE_MODE_RUN,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_SAFETY,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_HARDWARE,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_COMMUNICATION,

    //Kommunikation
    BPLC_PLI_KEY__SET_DEVICE_ADDRESS = 0x20,
    BPLC_PLI_KEY__DEBUG_MODE,

    //Hardware
    BPLC_PLI_KEY__DISABLE_EC_ERROR_DETECTION = 0x28,
    BPLC_PLI_KEY__ENABLE_DEBUG_OUTPUT = 0x29,
    BPLC_PLI_KEY__DEFINE_MCU = 0x30,
    BPLC_PLI_KEY__ADD_EC_AIN11revA,
    BPLC_PLI_KEY__ADD_EC_DIN11revA,
    BPLC_PLI_KEY__ADD_EC_DO11revA,
    BPLC_PLI_KEY__ADD_EC_REL11revA,
    BPLC_PLI_KEY__ADD_EC_MOT111revA,
    BPLC_PLI_KEY__ADD_EC_TMP11revA,
    BPLC_PLI_KEY__ADD_EC_PPO11revA,
    BPLC_PLI_KEY__ADD_EC_NANOrevA,
    BPLC_PLI_KEY__ADD_EC_FUSE12revA,

    BPLC_PLI_KEY__COUNT,

}e_BPLC_PLI_KEY_t;


void BPLC_APP::setupControlPanel()
{
    this->hostPc.begin(hostStartFrame, hostEndFrame);
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
                
                Serial.print("MCU:          "); Serial.println(this->APP_APP.settings.device.mcuCard);
                Serial.print("oledAvailable:"); Serial.println(this->APP_APP.settings.device.extensionCards.oledAvailable);              

                for(uint8_t card = 0; card < AIN11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.ain11revACards[card])
                    {
                        Serial.print("AIN11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }                    
                }                          
                for(uint8_t card = 0; card < DIN11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.din11revACards[card])
                    {
                        Serial.print("DIN11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }
                } 
                for(uint8_t card = 0; card < DO11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.do11revACards[card])
                    {
                        Serial.print("DO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < REL11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.rel11revACards[card])
                    {
                        Serial.print("REL11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < MOT11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.mot11revAcards[card])
                    {
                        Serial.print("MOT11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < TMP11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.tmp11revACards[card])
                    {
                        Serial.print("TMP11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < PPO11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.ppo11revACards[card])
                    {
                        Serial.print("PPO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                for(uint8_t card = 0; card < NANO11_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.nano11revACards[card])
                    {
                        Serial.print("NANO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                for(uint8_t card = 0; card < FUSE12_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.settings.device.extensionCards.fuse12revACards[card])
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
                this->loadDefaultParameter();
                this->saveDeviceSettings();
                Serial.println("RESET DEVICE TO CLEAR FLASH, reboot...");
                delay(2000);
                ESP.restart();
                break;

            case BPLC_PLI_KEY__RESET_EXTENSION_CARDS:
                memset(&this->APP_APP.settings.device.extensionCards, 0, sizeof(this->APP_APP.settings.device.extensionCards));
                this->saveDeviceSettings();
                Serial.println("RESET EXTENSIONCARDS, reboot...");
                delay(2000);
                ESP.restart();
                break;

            case BPLC_PLI_KEY__SOFT_RESET:
                Serial.println("SOFT RESET");
                delay(1000);
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
                this->setDeviceModeInternal(APP_MODE__STOP);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN:
                this->setDeviceModeInternal(APP_MODE__RUN);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_SAFETY:
                this->setDeviceModeInternal(APP_MODE__RUN_WITHOUT_SAFETY);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_HARDWARE:
                this->setDeviceModeInternal(APP_MODE__RUN_WITHOUT_EC_CARDS);
                break;

            case BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_COMMUNICATION:
                this->setDeviceModeInternal(APP_MODE__RUN_WITHOUT_COM);
                break;

            
//Kommunikation
            case BPLC_PLI_KEY__SET_DEVICE_ADDRESS:
                this->APP_APP.settings.device.communication.deviceAddress = COMMAND.command.payload;
                this->saveDeviceSettings();
                Serial.print("DEVICE ADDRESS SET TO "); Serial.println(this->APP_APP.settings.device.communication.deviceAddress);
                
                if(this->APP_COM.p_comNode == nullptr)
                {
                    //Warmstart
                    this->setupNetwork();
                }
                else
                {
                    Serial.println("NODE ALREADY DEFINED, device needs to reboot...");
                    delay(2000);
                    ESP.restart();
                }                
                break;

            case BPLC_PLI_KEY__DEBUG_MODE:
                if(1 == COMMAND.command.payload)
                {
                    this->APP_COM.p_comNode->enableDebugmode();
                }
                else
                {
                    this->APP_COM.p_comNode->disableDebugmode();
                }
                break;
 //Hardware  
            case BPLC_PLI_KEY__ENABLE_DEBUG_OUTPUT:
                if(this->ecmForSlowSpeed != nullptr)
                {    
                    this->ecmForSlowSpeed->enableECDebugOutput();
                } 
                if(this->ecmForHighSpeed != nullptr)
                {
                    this->ecmForHighSpeed->enableECDebugOutput();
                }                               
                break;

            case BPLC_PLI_KEY__DISABLE_EC_ERROR_DETECTION:
                if(this->ecmForSlowSpeed != nullptr)
                {    
                    this->ecmForSlowSpeed->disableECErrorDetection();
                } 
                if(this->ecmForHighSpeed != nullptr)
                {
                    this->ecmForHighSpeed->disableECErrorDetection();
                }   
                break;
                                    
            case BPLC_PLI_KEY__DEFINE_MCU:
                switch (COMMAND.command.payload)
                {
                    case 0:
                        this->APP_APP.settings.device.mcuCard = EC__MCU11revA;                
                        break;
                    case 1:
                        this->APP_APP.settings.device.mcuCard = EC__MCU11revB;                
                        break;
                    case 2:
                        this->APP_APP.settings.device.mcuCard = EC__MCU11revC;                
                        break;
                    
                    default:
                        break;
                }
                this->saveDeviceSettings();
                Serial.println("device needs to reboot...");
                delay(2000);
                ESP.restart();    
                break;     

            case BPLC_PLI_KEY__ADD_EC_AIN11revA:                
                this->APP_APP.settings.device.extensionCards.ain11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();             
                break;

            case BPLC_PLI_KEY__ADD_EC_DIN11revA:
                this->APP_APP.settings.device.extensionCards.din11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();             
                break;
            
            case BPLC_PLI_KEY__ADD_EC_DO11revA:
                this->APP_APP.settings.device.extensionCards.do11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();             
                break;       

            case BPLC_PLI_KEY__ADD_EC_REL11revA:
                this->APP_APP.settings.device.extensionCards.rel11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_MOT111revA:                                
                switch(COMMAND.command.payload)
                {
                    default:
                    //neue Mot11 hinzufügen
                        if(COMMAND.command.payload < MOT11_ADDRESS_COUNT)
                        {
                            this->APP_APP.settings.device.extensionCards.mot11revAcards[COMMAND.command.payload] = true;    
                            this->saveDeviceSettings();
                            this->setupHardware();
                        }
                        break;
                    case 4:
                        this->ecmForSlowSpeed->startCurrentTuningMot11(EC_ADDR_1);
                        break;
                    case 5:
                        this->ecmForSlowSpeed->startCurrentTuningMot11(EC_ADDR_2);
                        break;
                    case 6:
                        this->ecmForSlowSpeed->startCurrentTuningMot11(EC_ADDR_3);
                        break;
                    case 7:
                        this->ecmForSlowSpeed->startCurrentTuningMot11(EC_ADDR_4);
                        break;
                }                          
                break;

            case BPLC_PLI_KEY__ADD_EC_TMP11revA:
                this->APP_APP.settings.device.extensionCards.tmp11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_PPO11revA:
                this->APP_APP.settings.device.extensionCards.ppo11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_NANOrevA:
                this->APP_APP.settings.device.extensionCards.nano11revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__ADD_EC_FUSE12revA:
                this->APP_APP.settings.device.extensionCards.fuse12revACards[COMMAND.command.payload] = true;    
                this->saveDeviceSettings();
                this->setupHardware();
                break;        

            default:
                this->printLog("host command not executable!", __FILENAME__, __LINE__);
                break;
        }        
    }
}