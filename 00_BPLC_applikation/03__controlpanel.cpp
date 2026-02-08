#include "BPLC.h"

#define hostStartFrame  36  //ASCII $
#define hostEndFrame    37  //ASCII %

typedef enum
{   
    //System
    BPLC_PLI_KEY__GET_SYSTEM_ERRORS,
    BPLC_PLI_KEY__RESET_ALL_ERRORS,    
    BPLC_PLI_KEY__GET_DEVICE_SETTINGS,
    BPLC_PLI_KEY__RESET_ALL_SETTINGS,
    BPLC_PLI_KEY__PLATZHALTER_PLATZHALTER_PLATZHALTER,
    BPLC_PLI_KEY__SOFT_RESET,
    BPLC_PLI_KEY__INVERT_ENCODER,
    BPLC_PLI_KEY__USE_BUZZER,
    BPLC_PLI_KEY__BEEP_ON_ENCODER_INPUT,
    BPLC_PLI_KEY__GET_RAM_USAGE,
    BPLC_PLI_KEY__DISABLE_ERROR_DETECTION,

    //Applikation
    BPLC_PLI_KEY__DEVICE_MODE_STOP = 0x10,
    BPLC_PLI_KEY__DEVICE_MODE_RUN,    

    //Kommunikation
    BPLC_PLI_KEY__NETWORK_SET_DEVICE_ADDRESS = 0x20,
    BPLC_PLI_KEY__NETWORK_DEBUG_MODE,
    BPLC_PLI_KEY__NETWORK_RESET_ALL_NODES,
    BPLC_PLI_KEY__NETWORK_OBSERVE_CONNECTION,

    //Hardware
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


void BPLC::setupControlPanel()
{
    this->hostPc.begin(hostStartFrame, hostEndFrame);
}
void BPLC::tickControlPanel()
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

            case BPLC_PLI_KEY__DISABLE_ERROR_DETECTION:
                this->systemErrorManager.disableErrordetection(__FILENAME__, __LINE__);
                break;

            case BPLC_PLI_KEY__GET_DEVICE_SETTINGS:
                Serial.println("------SOFTWARE------");
                Serial.print("Version: ");      Serial.println(String(this->APP_APP.settings.device.application.versionMajor) + "V" + String(this->APP_APP.settings.device.application.versionMinor));
                
                Serial.println("------HARDWARE------");                
                Serial.print("MCU:          "); Serial.println(this->APP_APP.settings.device.mcuCard);
                Serial.print("oledAvailable:"); Serial.println(this->APP_APP.settings.device.extensionCards.oledAvailable);         
              
                Serial.println("------APPLIKATION SETTINGS------");
                Serial.print("encoder beep: ");         Serial.println(this->APP_APP.settings.device.application.f_beepOnEncoderInput);
                Serial.print("encoder inverted:  ");    Serial.println(this->APP_APP.settings.device.application.f_encoderInverted);
                Serial.print("buzzer used:  ");         Serial.println(this->APP_APP.settings.device.application.f_useBuzzer);

                Serial.println("------COMMUNICATION------");
                Serial.print("Device adddress: ");      Serial.println(this->APP_APP.settings.device.communication.deviceAddress);
               break;

            case BPLC_PLI_KEY__RESET_ALL_SETTINGS:
                this->parameterFlash.clear();
                this->setupParameterFlash();
                this->loadDefaultParameter();
                this->saveDeviceSettings();
                Serial.println("RESET DEVICE TO CLEAR FLASH, reboot...");
                this->triggerSoftReset();
                break;

            case BPLC_PLI_KEY__SOFT_RESET:
                Serial.println("SOFT RESET");
                this->triggerSoftReset();
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
            
//Kommunikation
            case BPLC_PLI_KEY__NETWORK_SET_DEVICE_ADDRESS:
                if(COMMAND.command.payload == 0)
                {
                    Serial.println("DISABLE NETWORK NODE, need to reboot");
                    this->APP_APP.settings.device.communication.deviceAddress = 0;
                    this->saveDeviceSettings();
                    this->triggerSoftReset();
                }                
                else if(this->APP_COM.p_comNode == nullptr)
                {
                    this->APP_APP.settings.device.communication.deviceAddress = COMMAND.command.payload;
                    this->saveDeviceSettings();
                    Serial.print("DEVICE ADDRESS SET TO "); Serial.println(this->APP_APP.settings.device.communication.deviceAddress);
                    //Warmstart
                    this->setupNetwork();
                }
                else
                {
                    Serial.println("NODE ALREADY DEFINED");
                }                
                break;

            case BPLC_PLI_KEY__NETWORK_DEBUG_MODE:
                if(this->APP_COM.p_comNode != nullptr)
                { 
                    this->APP_COM.p_comNode->enableDebugmode();          
                }
                break;

            case BPLC_PLI_KEY__NETWORK_RESET_ALL_NODES:
                if(this->APP_COM.p_comNode != nullptr)
                {    
                    this->APP_COM.p_comNode->resetAllNodes();
                }
                break;

            case BPLC_PLI_KEY__NETWORK_OBSERVE_CONNECTION:
                this->APP_APP.settings.device.communication.observeNetworkConnection = (!this->APP_APP.settings.device.communication.observeNetworkConnection);
                this->saveDeviceSettings();

                if(this->APP_APP.settings.device.communication.observeNetworkConnection)                
                {
                    Serial.println("OBSERVE NETWORK CONNECTION");
                }
                else
                {
                    Serial.println("DONT OBSERVE NETWORK CONNECTION");
                }
                break;
 //Hardware  
            case BPLC_PLI_KEY__ENABLE_DEBUG_OUTPUT:
                this->APP_APP.settings.device.extensionCards.debugOutputEnabled = (!this->APP_APP.settings.device.extensionCards.debugOutputEnabled);
                this->saveDeviceSettings();
                
                if(this->ecmForSlowSpeed != nullptr)
                {    
                    this->ecmForSlowSpeed->enableECDebugOutput();
                } 
                if(this->ecmForHighSpeed != nullptr)
                {
                    this->ecmForHighSpeed->enableECDebugOutput();
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
                this->triggerSoftReset();  
                break;     
          
            default:
                this->printLog("host command not executable!", __FILENAME__, __LINE__);
                break;
        }        
    }
}