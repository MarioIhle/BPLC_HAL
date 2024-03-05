#include "BPLC_ecManager.h"

BPLC_extensionCardManager::BPLC_extensionCardManager()
{}
void BPLC_extensionCardManager::mapObjectToExtensionCard(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const uint8_t CHANNEL)                                
{
    extensionCard* p_cardToMapChannelTo = this->searchExtensionCard(CARD);

    if(p_cardToMapChannelTo != nullptr)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }
    else
    {//Error Setzen
        switch (CARD)
        {
            case BPLC_CARD__AIN11revA_1:    
                this->setError(ECM_ERROR__AIN11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__AIN11revA_2:    
                this->setError(ECM_ERROR__AIN11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__AIN11revA_3:    
                this->setError(ECM_ERROR__AIN11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__AIN11revA_4:   
                this->setError(ECM_ERROR__AIN11revA_ADDR_4_NOT_DEFINED);
             break; 

            case BPLC_CARD__DIN11revA_1:  
                this->setError(ECM_ERROR__DIN11revA_ADDR_1_NOT_DEFINED);
            break; 
            case BPLC_CARD__DIN11revA_2:  
                this->setError(ECM_ERROR__DIN11revA_ADDR_2_NOT_DEFINED);
            break;
            case BPLC_CARD__DIN11revA_3:  
                this->setError(ECM_ERROR__DIN11revA_ADDR_3_NOT_DEFINED);  
            break; 
            case BPLC_CARD__DIN11revA_4:  
                this->setError(ECM_ERROR__DIN11revA_ADDR_4_NOT_DEFINED);
            break; 

            case BPLC_CARD__DO11revA_1:  
                this->setError(ECM_ERROR__DO11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__DO11revA_2:    
                this->setError(ECM_ERROR__DO11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__DO11revA_3:    
                this->setError(ECM_ERROR__DO11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__DO11revA_4:    
                this->setError(ECM_ERROR__DO11revA_ADDR_4_NOT_DEFINED);
            break;            

            case BPLC_CARD__REL11revA_1:    
                this->setError(ECM_ERROR__REL11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__REL11revA_2:    
                this->setError(ECM_ERROR__REL11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__REL11revA_3:    
                this->setError(ECM_ERROR__REL11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__REL11revA_4:    
                this->setError(ECM_ERROR__REL11revA_ADDR_4_NOT_DEFINED);
            break; 

            case BPLC_CARD__MOT11revA_1:    
                this->setError(ECM_ERROR__MOT11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__MOT11revA_2:    
                this->setError(ECM_ERROR__MOT11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__MOT11revA_3:    
                this->setError(ECM_ERROR__MOT11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__MOT11revA_4:    
                this->setError(ECM_ERROR__MOT11revA_ADDR_4_NOT_DEFINED);
            break; 

            case BPLC_CARD__TMP11revA_1:   
                this->setError(ECM_ERROR__TMP11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__TMP11revA_2:   
                this->setError(ECM_ERROR__TMP11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__TMP11revA_3:   
                this->setError(ECM_ERROR__TMP11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__TMP11revA_4:   
                this->setError(ECM_ERROR__TMP11revA_ADDR_4_NOT_DEFINED);
            break;

            case BPLC_CARD__NANO11revA_1:   
                this->setError(ECM_ERROR__NANO11revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__NANO11revA_2:    
                this->setError(ECM_ERROR__NANO11revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__NANO11revA_3:   
                this->setError(ECM_ERROR__NANO11revA_ADDR_4_NOT_DEFINED); 
            break; 
            case BPLC_CARD__NANO11revA_4:    
                this->setError(ECM_ERROR__NANO11revA_ADDR_4_NOT_DEFINED);
            break;  

            case BPLC_CARD__FUSE12revA_1:   
                this->setError(ECM_ERROR__FUSE12revA_ADDR_4_NOT_DEFINED); 
            break; 
            case BPLC_CARD__FUSE12revA_2:    
                this->setError(ECM_ERROR__FUSE12revA_ADDR_4_NOT_DEFINED);
            break; 
            case BPLC_CARD__FUSE12revA_3:    
                this->setError(ECM_ERROR__FUSE12revA_ADDR_4_NOT_DEFINED);
            break;
            case BPLC_CARD__FUSE12revA_4:    
                this->setError(ECM_ERROR__FUSE12revA_ADDR_4_NOT_DEFINED);
            break;                  
        
            default:
                this->setError(ECM_ERROR__EC_NOT_DEFINED);
            break;
        }
    }      
}  
bool BPLC_extensionCardManager::i2cAddressIsUsedByExtensionCard(const uint8_t I2C_ADDRESS)
{
    const bool I2C_ADDRESS_IS_USED_BY_ACTIVE_EC = (bool)this->searchExtensionCard((e_BPLC_CARD_TYPE_t)I2C_ADDRESS);
    return I2C_ADDRESS_IS_USED_BY_ACTIVE_EC;
}
bool BPLC_extensionCardManager::addNewExtensionCard(const e_BPLC_CARD_TYPE_t EXTENSION_CARD_TYPE)
{
    bool newEcAdded = false;   
    if(this->searchExtensionCard(EXTENSION_CARD_TYPE) == nullptr)
    {
        halInterface* p_newHalInterface;
        switch (EXTENSION_CARD_TYPE)
        {
            //MCU
            case BPLC_CARD__MCU11revA:    
                p_newHalInterface = new HAL_MCU11_revA(&this->isrCount);  
                break;
            case BPLC_CARD__MCU11revB:    
            case BPLC_CARD__MCU11revC://Gleiches pinning, nur änderungen im Layout 
                p_newHalInterface = new HAL_MCU11_revB(&this->isrCount);      
                break;        
            //DIN11revA
            case BPLC_CARD__DIN11revA_1:
                p_newHalInterface = new HAL_DIN11(DIN11_CARD_1_ADDRESS);                                     
                break;
            case BPLC_CARD__DIN11revA_2:
                p_newHalInterface = new HAL_DIN11(DIN11_CARD_2_ADDRESS);                           
                break;
            case BPLC_CARD__DIN11revA_3:
                p_newHalInterface = new HAL_DIN11(DIN11_CARD_3_ADDRESS);                           
                break;
            case BPLC_CARD__DIN11revA_4:
                p_newHalInterface = new HAL_DIN11(DIN11_CARD_4_ADDRESS);                           
                break;
            //DO11revA
            case BPLC_CARD__DO11revA_1:
                p_newHalInterface = new HAL_DO11(DO11_CARD_1_ADDRESS);               
                break;
            case BPLC_CARD__DO11revA_2:
                p_newHalInterface = new HAL_DO11(DO11_CARD_2_ADDRESS);               
                break;
            case BPLC_CARD__DO11revA_3:
                p_newHalInterface = new HAL_DO11(DO11_CARD_3_ADDRESS);               
                break;
            case BPLC_CARD__DO11revA_4:
                p_newHalInterface = new HAL_DO11(DO11_CARD_4_ADDRESS);               
                break;
            //AIN11revA
            case BPLC_CARD__AIN11revA_1:
                p_newHalInterface = new HAL_AIN11(AIN11_CARD_1_ADDRESS); 
                break;
            case BPLC_CARD__AIN11revA_2:
                p_newHalInterface = new HAL_AIN11(AIN11_CARD_2_ADDRESS); 
                break;
            case BPLC_CARD__AIN11revA_3:
                p_newHalInterface = new HAL_AIN11(AIN11_CARD_3_ADDRESS); 
                break;
            case BPLC_CARD__AIN11revA_4:
                p_newHalInterface = new HAL_AIN11(AIN11_CARD_4_ADDRESS); 
                break;
            //REL11revA
            case BPLC_CARD__REL11revA_1:
                p_newHalInterface = new HAL_REL11(REL11_CARD_1_ADDRESS); 
                break;
            case BPLC_CARD__REL11revA_2:
                p_newHalInterface = new HAL_REL11(REL11_CARD_2_ADDRESS); 
                break;
            case BPLC_CARD__REL11revA_3:
                p_newHalInterface = new HAL_REL11(REL11_CARD_3_ADDRESS); 
                break;
            case BPLC_CARD__REL11revA_4:
                p_newHalInterface = new HAL_REL11(REL11_CARD_4_ADDRESS); 
                break;
            //MOT11revA
            case BPLC_CARD__MOT11revA_1:
                p_newHalInterface = new HAL_MOT11(MOT11_CARD_1_ADDRESS); 
                break;
            case BPLC_CARD__MOT11revA_2:
                p_newHalInterface = new HAL_MOT11(MOT11_CARD_2_ADDRESS); 
                break;
            case BPLC_CARD__MOT11revA_3:
                p_newHalInterface = new HAL_MOT11(MOT11_CARD_3_ADDRESS); 
                break;
            case BPLC_CARD__MOT11revA_4:
                p_newHalInterface = new HAL_MOT11(MOT11_CARD_4_ADDRESS); 
                break;
            //AIN11revA
            case BPLC_CARD__TMP11revA_1:
                p_newHalInterface = new HAL_TMP11(TMP11_CARD_1_ADDRESS); 
                break;
            case BPLC_CARD__TMP11revA_2:
                p_newHalInterface = new HAL_TMP11(TMP11_CARD_2_ADDRESS); 
                break;
            case BPLC_CARD__TMP11revA_3:
                p_newHalInterface = new HAL_TMP11(TMP11_CARD_3_ADDRESS); 
                break;
            case BPLC_CARD__TMP11revA_4:
                p_newHalInterface = new HAL_TMP11(TMP11_CARD_4_ADDRESS); 
                break;
                
            default:
            case BPLC_CARD__NO_CARD_DEFINED:
                this->printResetReason("extensionCardManager", "addNewExtensionCard", "CARD NOT DEFINED");
                abort();
                break;
        }         
        
        p_newHalInterface->init();    
        //Neues extensionCard Objekt erzeugen und hal zuweisen
        if(p_newHalInterface->getErrorCode() == BPLC_ERROR__NO_ERROR)
        {
            extensionCard* p_extensionCard = new extensionCard();
            p_extensionCard->setHalInterface(p_newHalInterface);
            p_extensionCard->setCardType(EXTENSION_CARD_TYPE);   
            this->addExtensionCardToList(p_extensionCard);  
            newEcAdded = true;
        }         
        else
        {
            this->setError(p_newHalInterface->getErrorCode());
        }   
    }
    else
    {
        this->setError(ECM_ERROR__EC_ALREADY_DEFINED);
    }    
    return newEcAdded;
}
extensionCard* BPLC_extensionCardManager::searchExtensionCard(const e_BPLC_CARD_TYPE_t  SEARCHED_EXTENSION_CARD)
{
    extensionCard* p_searchedCard = this->p_firstExtensionCard;

    while (p_searchedCard != nullptr)
    {
        if(p_searchedCard->getCardType() == SEARCHED_EXTENSION_CARD)
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
                    case BPLC_CARD__DIN11revA_1:
                    case BPLC_CARD__DIN11revA_2:
                    case BPLC_CARD__DIN11revA_3:
                    case BPLC_CARD__DIN11revA_4:
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
                this->setError(EC_HAL_ERROR);
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