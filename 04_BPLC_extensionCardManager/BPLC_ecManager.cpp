#include "BPLC_ecManager.h"

BPLC_extensionCardHandler::BPLC_extensionCardHandler()
{}
void BPLC_extensionCardHandler::mapObjectToExtensionCard(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const uint8_t CHANNEL)                                
{
    extensionCard* p_cardToMapChannelTo = this->searchExtensionCard(CARD);

    if(p_cardToMapChannelTo != nullptr)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }
    else
    {
        p_cardToMapChannelTo = this->addNewExtensionCard(CARD);
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }      
}  
extensionCard* BPLC_extensionCardHandler::addNewExtensionCard(const e_BPLC_CARD_TYPE_t EXTENSION_CARD_TYPE)
{  
    halInterface* p_newHalInterface;

    switch (EXTENSION_CARD_TYPE)
    {
        //MCU
        case BPLC_CARD__MCU11revA:    
            p_newHalInterface = new HAL_MCU11_revA();  
            p_newHalInterface->mapObjectToChannel(&this->isrCounter, MCU_CHANNEL__INT);
            break;
        case BPLC_CARD__MCU11revB:     
            p_newHalInterface = new HAL_MCU11_revB();      
            p_newHalInterface->mapObjectToChannel(&this->isrCounter, MCU_CHANNEL__INT); 
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
        //AIN11revA
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
        //AIN11revA
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
            
        default:
        case BPLC_CARD__TYPE_COUNT:
        case BPLC_CARD__NO_CARD_DEFINED:
        case MCU_CARD__NO_MCU_DEFINED:
            this->printResetReason("extensionCardManager", "addNewExtensionCard", "CARD NOT DEFINED");
            abort();
            break;
    }         
    //Neues extensionCard Objekt erzeugen und hal zuweisen
    p_newHalInterface->init();
    extensionCard* p_extensionCard = new extensionCard();
    p_extensionCard->setHalInterface(p_newHalInterface);
    p_extensionCard->setCardType(EXTENSION_CARD_TYPE);              
    this->addExtensionCardToList(p_extensionCard);  

    return p_extensionCard;    	
}
extensionCard* BPLC_extensionCardHandler::searchExtensionCard(const e_BPLC_CARD_TYPE_t  SEARCHED_EXTENSION_CARD)
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
void BPLC_extensionCardHandler::addExtensionCardToList(extensionCard* CARD_TO_ADD)
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
void BPLC_extensionCardHandler::tick()
{
    if(this->p_firstExtensionCard!= nullptr)
    {
        extensionCard* p_extesionCardToTick = this->p_firstExtensionCard;
       
        while(p_extesionCardToTick != nullptr)
        {
            e_BPLC_ERROR_t EC_HAL_ERROR = p_extesionCardToTick->getHalInterface()->getErrorCode();
  
            if(EC_HAL_ERROR == BPLC_ERROR__NO_ERROR)
            {
                switch(p_extesionCardToTick->getCardType())            
                {
                    case BPLC_CARD__DIN11revA_1:
                    case BPLC_CARD__DIN11revA_2:
                    case BPLC_CARD__DIN11revA_3:
                    case BPLC_CARD__DIN11revA_4:
                        if(this->isrCounter.getCount() > 0)
                        {
                            p_extesionCardToTick->getHalInterface()->tick();
                            this->isrCounter.decrement();//Es könnten Flanken überschieben werden, dabei counter >1 mehrmals die Hal callbacks aufgerufen werden!!!!, aber gerade keine bessere Lösung vorhanden
                        }
                    break;

                    default:
                        p_extesionCardToTick->getHalInterface()->tick();
                    break;
                }          
            }  
            else
            {
                this->setError(EC_HAL_ERROR);
            }
            p_extesionCardToTick = p_extesionCardToTick->getNext();      
        }
    }
}