#include "BPLC_ecManager.h"

BPLC_extensionCardManager::BPLC_extensionCardManager()
{}
void BPLC_extensionCardManager::mapObjectToExtensionCard(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const e_EC_ADDR_t ADDR, const uint8_t CHANNEL)                                
{
    extensionCard* p_cardToMapChannelTo = this->searchExtensionCard(CARD, ADDR);

    if(p_cardToMapChannelTo != nullptr)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }
    else
    {//Error Setzen
        switch (CARD)
        {
            case BPLC_CARD__AIN11revA:
                switch (ADDR)
                {
                    case 0:
                        this->setError(ECM_ERROR__AIN11revA_ADDR_1_NOT_DEFINED, __FILENAME__, __LINE__);
                        break;
                    
                    default:
                        break;
                    }           
                break;

            default:
                this->setError(ECM_ERROR__EC_NOT_DEFINED, __FILENAME__, __LINE__);
            break;
        }
    }      
}  
bool BPLC_extensionCardManager::i2cAddressIsUsedByExtensionCard(const uint8_t I2C_ADDRESS)
{
    return false;
}
bool BPLC_extensionCardManager::addNewExtensionCard(const e_BPLC_CARD_TYPE_t EXTENSION_CARD_TYPE, const e_EC_ADDR_t ADDR)
{
    bool newEcAdded = false;   
    if(this->searchExtensionCard(EXTENSION_CARD_TYPE, ADDR) == nullptr)
    {
        halInterface* p_newHalInterface;
        
        switch (EXTENSION_CARD_TYPE)
        {
            case BPLC_CARD__MCU11revA:    
                p_newHalInterface = new HAL_MCU11_revA(&this->isrCount);  
                break;

            case BPLC_CARD__MCU11revB:    
            case BPLC_CARD__MCU11revC://Gleiches pinning, nur änderungen im Layout 
                p_newHalInterface = new HAL_MCU11_revB(&this->isrCount);      
                break;   

            case BPLC_CARD__AIN11revA:          
                p_newHalInterface = new HAL_AIN11();    
                break; 
       
            case BPLC_CARD__DIN11revA:              
                p_newHalInterface = new HAL_DIN11();
                break;                         
  
            case BPLC_CARD__DO11revA:        
                p_newHalInterface = new HAL_DO11();    
                break;                 

            case BPLC_CARD__REL11revA:                
                p_newHalInterface = new HAL_REL11();
                break;

            case BPLC_CARD__MOT11revA:                
                p_newHalInterface = new HAL_MOT11();                                                        
                break;
   
            case BPLC_CARD__TMP11revA:                
                p_newHalInterface = new HAL_TMP11();                                                                         
                break;
                
            default:
            case BPLC_CARD__NO_CARD_DEFINED:
                this->printResetReason("CARD NOT DEFINED", __FILENAME__, __LINE__);
                abort();
                break;
        }         
        //Hal initialisieren
        p_newHalInterface->init(ADDR);  

        //Neues extensionCard Objekt erzeugen und in Liste aufnehmen
        if(p_newHalInterface->getErrorCode() == BPLC_ERROR__NO_ERROR)
        {
            extensionCard* p_extensionCard = new extensionCard();
            p_extensionCard->setHalInterface(p_newHalInterface);
            p_extensionCard->setCardType(EXTENSION_CARD_TYPE);  
            p_extensionCard->setAddr(ADDR); 
            this->addExtensionCardToList(p_extensionCard);  
            newEcAdded = true;
        }         
        else
        {
            this->setError(p_newHalInterface->getErrorCode(), __FILENAME__, __LINE__);
        }   
    }
    else
    {
        this->printLog("EC ALREADY DEFINDED", __FILENAME__, __LINE__);
    }    
    return newEcAdded;
}
extensionCard* BPLC_extensionCardManager::searchExtensionCard(const e_BPLC_CARD_TYPE_t SEARCHED_EXTENSION_CARD, const e_EC_ADDR_t ADDR)
{
    extensionCard* p_searchedCard = this->p_firstExtensionCard;

    while (p_searchedCard != nullptr)
    {
        if(p_searchedCard->getCardType() == SEARCHED_EXTENSION_CARD && p_searchedCard->getAddr() == ADDR)
        {
            return p_searchedCard;
        }
        p_searchedCard = p_searchedCard->getNext();
    }

    return nullptr;
}
void BPLC_extensionCardManager::addExtensionCardToList(extensionCard* CARD_TO_ADD)
{    
    if(this->p_firstExtensionCard == nullptr)
    {
        this->p_firstExtensionCard = CARD_TO_ADD;
    }
    else
    {
        CARD_TO_ADD->setNext(this->p_firstExtensionCard);
        this->p_firstExtensionCard = CARD_TO_ADD;
    }  
}
void BPLC_extensionCardManager::tick()
{
    if(this->p_firstExtensionCard!= nullptr)
    {
        extensionCard* p_extensionCardToTick = this->p_firstExtensionCard;
    
        while(p_extensionCardToTick != nullptr)
        {
            e_BPLC_ERROR_t EC_HAL_ERROR = p_extensionCardToTick->getHalInterface()->getErrorCode();

            if(EC_HAL_ERROR == BPLC_ERROR__NO_ERROR)
            {
                switch(p_extensionCardToTick->getCardType())            
                {
                    case BPLC_CARD__DIN11revA:
                        if(this->isrCount > 0)
                        {
                            p_extensionCardToTick->getHalInterface()->tick();                           
                        }
                    break;

                    default:
                        p_extensionCardToTick->getHalInterface()->tick();
                    break;
                }      
            }  
            else
            {
                this->setError(EC_HAL_ERROR, __FILENAME__, __LINE__);
            }
            p_extensionCardToTick = p_extensionCardToTick->getNext();      
        }
        //nach lesen ALLER (DIN) Karten count verringern
        if(this->isrCount > 0)
        {                  
            this->isrCount--;
        }  
    }
      
}