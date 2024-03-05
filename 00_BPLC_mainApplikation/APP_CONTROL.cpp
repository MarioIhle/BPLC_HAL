#include "BPLC_APP.h"

void BPLC_APP::setupControlPanel()
{

}
void BPLC_APP::tickControlPanel()
{
    if(this->hostPc.available())
    {
        switch (this->hostPc.getCommand().command.key)
        {
            case BPLC_PLI_KEY__GET_SETTINGS:
                Serial.println("HARDWARE SETTINGS: ");
                
                Serial.print("MCU:          "); Serial.println(this->APP_APP.device.settings.mcuCard);
                Serial.print("oledAvailable:"); Serial.println(this->APP_APP.device.settings.oledAvailable);              

                for(uint8_t card = 0; card < AIN11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.ain11revACards[card])
                    {
                        Serial.print("AIN11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }                    
                }                          
                for(uint8_t card = 0; card < DIN11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.din11revACards[card])
                    {
                        Serial.print("DIN11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }
                } 
                for(uint8_t card = 0; card < DO11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.do11revACards[card])
                    {
                        Serial.print("DO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < REL11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.rel11revACards[card])
                    {
                        Serial.print("REL11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < MOT11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.mot11revAcards[card])
                    {
                        Serial.print("MOT11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < TMP11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.tmp11revACards[card])
                    {
                        Serial.print("TMP11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                } 
                for(uint8_t card = 0; card < PPO11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.ppo11revACards[card])
                    {
                        Serial.print("PPO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                for(uint8_t card = 0; card < NANO11_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.nano11revACards[card])
                    {
                        Serial.print("NANO11revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                for(uint8_t card = 0; card < FUSE12_CARD_ADDRESS_COUNT; card++)
                {
                    if(this->APP_APP.device.settings.fuse12revACards[card])
                    {
                        Serial.print("FUSE12revA ADDR "); Serial.print(card); Serial.println(" defined ");
                    }           
                }
                Serial.println("APPLIKATION SETTINGS:");
                Serial.print("encoder beep: "); Serial.println(this->APP_APP.device.settings.f_beepOnEncoderInput);
                Serial.print("buzzer used:  "); Serial.println(this->APP_APP.device.settings.f_useBuzzer);
                break;

            case BPLC_PLI_KEY__RESET_ALL_SETTINGS:
                this->parameterFlash.clear();
                this->setupParameterFlash();
                break;
                
            case BPLC_PLI_KEY__SOFT_RESET:
                abort();
                break;

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

            case BPLC_PLI_KEY__GET_SYSTEM_ERRORS:
                for(uint8_t error = 0; error < HARDWARE_ERROR_BUFFER_SIZE; error++)
                {
                    this->printError(String(error+1) + ". SYSTEM ERROR CODE: " + String(this->getSystemErrorCode(error)) + ", " + String(this->APP_SAFETY.errorOut.getErrorCodeText(this->getSystemErrorCode(error)))); 
                }
                break;

            case BPLC_PLI_KEY__RESET_ALL_ERRORS:
                for(uint8_t error = 0; error < HARDWARE_ERROR_BUFFER_SIZE; error++)
                {
                    this->resetError(error);
                }
                break;

            case BPLC_PLI_KEY__DEFINE_MCU11revA:
                this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MCU11revA);
                break;

            case BPLC_PLI_KEY__DEFINE_MCU11revB:
                this->APP_APP.device.settings.mcuCard = BPLC_CARD__MCU11revB;
                this->saveDeviceSettings();
                this->setupHardware();
                break;

            case BPLC_PLI_KEY__DEFINE_MCU11revC:
                this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MCU11revC);
                break;
            

            default:
                this->printLog("host command not executable!");
                break;
        }        
    }
}