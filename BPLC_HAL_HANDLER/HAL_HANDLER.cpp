#include "HAL_HANDLER.h"

BPLC_extensionCardHandler::BPLC_extensionCardHandler()
{}
void BPLC_extensionCardHandler::addNewextensionCard(const e_EXTENSION_CARD_TYPE_t EXTENSION_CARD_TYPE)
{
    if(this->searchextensionCard(EXTENSION_CARD_TYPE) == nullptr)
    {
        halInterface* p_newHalInterface;

        switch (EXTENSION_CARD_TYPE)
        {
            default:
            case EXTENSION_CARD__NO_CARD_DEFINED:
            break;
        
            case EXTENSION_CARD__DIN11revA_1:
            case EXTENSION_CARD__DIN11revA_2:
            case EXTENSION_CARD__DIN11revA_3:
            case EXTENSION_CARD__DIN11revA_4:
                p_newHalInterface = new HAL_DIN11();                           
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
                p_newHalInterface = new HAL_FUSE11(); 
            break;

            case EXTENSION_CARD__FUSE12revA_1:   
            case EXTENSION_CARD__FUSE12revA_2:     
            case EXTENSION_CARD__FUSE12revA_3:   
            case EXTENSION_CARD__FUSE12revA_4:
                //p_newHalInterface = new HAL_FUSE12(); 
            break;
        }
        
        p_newHalInterface->begin();
        extensionCard* p_extensionCard = new extensionCard();
        p_extensionCard->setHalInterface(p_newHalInterface);
        p_extensionCard->setCardType(EXTENSION_CARD_TYPE);
        this->addExtensionCardToList(p_extensionCard);        	
    }
}
extensionCard* BPLC_extensionCardHandler::searchextensionCard(const e_EXTENSION_CARD_TYPE_t SEARCHED_EXTENSION_CARD)
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
    extensionCard* p_halToTick = this->p_firstextensionCard;
    while (p_halToTick != nullptr)
    {
        p_halToTick->getHalInterface()->tick();
        p_halToTick = p_halToTick->getNext();
    }
}
void BPLC_extensionCardHandler::mapextensionCardChannel(e_EXTENSION_CARD_TYPE_t CARD, uint8_t CHANNEL)
{
    extensionCard* p_cardToMapChannelTo = this->searchextensionCard(CARD);
    if(p_cardToMapChannelTo != nullptr)
    {   
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(&P_OBJEKT, CHANNEL)
    }
}   