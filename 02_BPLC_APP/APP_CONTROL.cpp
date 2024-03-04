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
        case BPLC_PLI_KEY__DEFINE_MCU11revA:
            this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MCU11revA);
            break;
        case BPLC_PLI_KEY__DEFINE_MCU11revB:
            this->extensionCardHandler.addNewExtensionCard(BPLC_CARD__MCU11revB);
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