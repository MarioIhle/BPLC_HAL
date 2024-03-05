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
                this->printError("SYSTEM ERROR CODE SET: " + String(this->getFirstSystemErrorCode()) + ", " + String(this->APP_SAFETY.errorOut.getErrorCodeText(this->getFirstSystemErrorCode()))); 
                break;
            case BPLC_PLI_KEY__RESET_ALL_ERRORS:
                this->resetLastError();
                break;
            case BPLC_PLI_KEY__DEFINE_MCU11revA:
                this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MCU11revA);
                break;
            case BPLC_PLI_KEY__DEFINE_MCU11revB:
                this->APP_APP.deviceSettings.hardware.mcuCard = BPLC_CARD__MCU11revB;
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