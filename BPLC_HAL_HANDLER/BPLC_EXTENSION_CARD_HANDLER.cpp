#include "BPLC_EXTENSION_CARD_HANDLER.h"

BPLC_extensionCardHandler::BPLC_extensionCardHandler()
{}
extensionCard* BPLC_extensionCardHandler::addNewExtensionCard(const e_EXTENSION_CARD_TYPE_t EXTENSION_CARD_TYPE)
{  
    halInterface* p_newHalInterface;

    switch (EXTENSION_CARD_TYPE)
    {
        default:
        case EXTENSION_CARD__NO_CARD_DEFINED:
        break;
    
        case EXTENSION_CARD__DIN11revA_1:
            p_newHalInterface = new HAL_DIN11(DIN11_CARD_1_ADDRESS);                           
            break;
        case EXTENSION_CARD__DIN11revA_2:
            p_newHalInterface = new HAL_DIN11(DIN11_CARD_2_ADDRESS);                           
            break;
        case EXTENSION_CARD__DIN11revA_3:
            p_newHalInterface = new HAL_DIN11(DIN11_CARD_3_ADDRESS);                           
            break;
        case EXTENSION_CARD__DIN11revA_4:
            p_newHalInterface = new HAL_DIN11(DIN11_CARD_4_ADDRESS);                           
        break;

        case EXTENSION_CARD__DO11revA_1:
        case EXTENSION_CARD__DO11revA_2:
        case EXTENSION_CARD__DO11revA_3:
        case EXTENSION_CARD__DO11revA_4:
            p_newHalInterface = new HAL_DO11();               
        break;

        case EXTENSION_CARD__AIN11revA_1:
        case EXTENSION_CARD__AIN11revA_2:
        case EXTENSION_CARD__AIN11revA_3:
        case EXTENSION_CARD__AIN11revA_4:
            p_newHalInterface = new HAL_AIN11(); 
        break;

        case EXTENSION_CARD__REL11revA_1:
        case EXTENSION_CARD__REL11revA_2:
        case EXTENSION_CARD__REL11revA_3:
        case EXTENSION_CARD__REL11revA_4:
            p_newHalInterface = new HAL_REL11(); 
        break;

        case EXTENSION_CARD__MOT11revA_1:
        case EXTENSION_CARD__MOT11revA_2:
        case EXTENSION_CARD__MOT11revA_3:
        case EXTENSION_CARD__MOT11revA_4:
            p_newHalInterface = new HAL_MOT11(); 
        break;

        case EXTENSION_CARD__NANO11revA_1:
        case EXTENSION_CARD__NANO11revA_2:
        case EXTENSION_CARD__NANO11revA_3:
        case EXTENSION_CARD__NANO11revA_4:
            //p_newHalInterface = new HAL_NANO11(); 
        break;
        
        case EXTENSION_CARD__FUSE11revA_1:
        case EXTENSION_CARD__FUSE11revA_2:
        case EXTENSION_CARD__FUSE11revA_3:
        case EXTENSION_CARD__FUSE11revA_4:
            //p_newHalInterface = new HAL_FUSE11(); 
        break;

        case EXTENSION_CARD__FUSE12revA_1:   
        case EXTENSION_CARD__FUSE12revA_2:     
        case EXTENSION_CARD__FUSE12revA_3:   
        case EXTENSION_CARD__FUSE12revA_4:
            //p_newHalInterface = new HAL_FUSE12(); 
        break;        
        
        p_newHalInterface->begin(EXTENSION_CARD_TYPE);
        extensionCard* p_extensionCard = new extensionCard();
        p_extensionCard->setHalInterface(p_newHalInterface);
        p_extensionCard->setCardType(EXTENSION_CARD_TYPE);
        this->addExtensionCardToList(p_extensionCard);  

        return p_extensionCard;      	
    }
}
extensionCard* BPLC_extensionCardHandler::searchExtensionCard(const e_EXTENSION_CARD_TYPE_t  SEARCHED_EXTENSION_CARD)
{
    extensionCard* p_searchedCard = this->p_firstextensionCard;

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
void BPLC_extensionCardHandler::addextensionCardToList(extensionCard* CARD_TO_ADD)
{    
    if(this->p_firstextensionCard == nullptr)
    {
        this->p_firstextensionCard = CARD_TO_ADD;
    }
    else
    {
        CARD_TO_ADD->setNext(this->p_firstextensionCard);
        this->p_firstextensionCard = CARD_TO_ADD;
    }  
}
void BPLC_extensionCardHandler::tickAllextensionCards()
{
    extensionCard* p_extesionCardToTick = this->p_firstextensionCard;
    while(p_extesionCardToTick != nullptr)
    {
        this->setError(p_extesionCardToTick->getHalInterface()->getError());
        if(this->getError() == BPLC_ERROR__NO_ERROR)
        {
            p_extesionCardToTick->getHalInterface()->tick();            
        }  
        p_extesionCardToTick = p_extesionCardToTick->getNext();      
    }
}
void BPLC_extensionCardHandler::mapExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_EXTENSION_CARD_TYPE_t CARD, const uint8_t CHANNEL)                                
{
    extensionCard* p_cardToMapChannelTo = this->searchExtensionCard(CARD);

    if(p_cardToMapChannelTo != nullptr)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }
    else
    {
        p_cardToMapChannelTo = addNewExtensionCard(CARD);
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }
}   