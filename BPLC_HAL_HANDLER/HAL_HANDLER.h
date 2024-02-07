#ifndef HAL_HANDLER_h
#define HAL_HANDLER_h

#include "Arduino.h"
#include "BPLC_CARDS.h"

//Datenelement
class halInterface
{
    public:
    virtual void setup()    = 0;
    virtual void tick()     = 0;
    virtual void getError() = 0;
    virtual void mapObjectToChannel() = 0;
};
//Knoten
class extensionCard
{    
    public:
                            extensionCard   (){this->CARD_TYPE = EXTENSION_CARD__NO_CARD_DEFINED;}
    void                    setNext         (extensionCard* P_NEXT){this->p_next = P_NEXT;}
    extensionCard*          getNext         (){return this->p_next;}
    halInterface*           getHalInterface (){return this->p_hal;}
    void                    setHalInterface (halInterface* P_HAL_INTERFACE){this->p_hal =P_HAL_INTERFACE;}
    void                    setCardType     (const e_EXTENSION_CARD_TYPE_t CARD_TYPE){this->cardType = CARD_TYPE};
    e_EXTENSION_CARD_TYPE_t getCardType     (){return this->cardType;}

    private:
    extensionCard*          p_next;
    halInterface*           p_hal;
    e_EXTENSION_CARD_TYPE_t cardType; 
};
//Warteschlange
class BPLC_extensionCardHandler
{
    public:
            BPLC_extensionCardHandler   ();
    void    addNewExtensionCard         (const e_EXTENSION_CARD_TYPE_t EXTENSION_CARD_TYPE);
    void    tickAllExtensionCards       ();
    void    mapExtensionCardChannel     (const e_EXTENSION_CARD_TYPE_t, uint8_t CHANNEL);


    private:
    void            addextensionCardToList  (extensionCard* CARD_TO_ADD);
    extensionCard*  searchExtensionCard     (const e_EXTENSION_CARD_TYPE_t  SEARCHED_EXTENSION_CARD);
    extensionCard*  p_firstExtensionCard;    
};
#endif