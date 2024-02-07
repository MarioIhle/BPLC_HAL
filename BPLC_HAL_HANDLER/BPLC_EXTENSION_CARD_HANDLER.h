#ifndef BPLC_EXTENSION_CARD_HANDLER_h
#define BPLC_EXTENSION_CARD_HANDLER_h

#include "Arduino.h"
#include "BPLC_CARDS.h"
#include "BPLC_IOM.h"
#include "BPLC_ERRORS.h"
//HAL´s
#include "HAL_DIN11.h"
#include "HAL_AIN11.h"
#include "HAL_REL11.h"
#include "HAL_FUSE11.h"
#include "HAL_DO11.h"
#include "HAL_MOT11.h"

//Datenelement
class halInterface
{
    public:
    virtual void            setup()    = 0;
    virtual void            tick()     = 0;
    virtual e_BPLC_ERROR_t  getError() = 0;
    virtual void            mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL) = 0;
};
//Knoten
class extensionCard
{    
    public:
                            extensionCard   (){this->cardType = EXTENSION_CARD__NO_CARD_DEFINED;}
    void                    setNext         (extensionCard* P_NEXT){this->p_next = P_NEXT;}
    extensionCard*          getNext         (){return this->p_next;}
    halInterface*           getHalInterface (){return this->p_hal;}
    void                    setHalInterface (halInterface* P_HAL_INTERFACE){this->p_hal =P_HAL_INTERFACE;}
    void                    setCardType     (const e_EXTENSION_CARD_TYPE_t CARD_TYPE){this->cardType = CARD_TYPE;}
    e_EXTENSION_CARD_TYPE_t getCardType     (){return this->cardType;}

    private:
    extensionCard*          p_next;
    halInterface*           p_hal;
    e_EXTENSION_CARD_TYPE_t cardType; 
};
//Warteschlange
class BPLC_extensionCardHandler:BPLC_errorHandler
{
    public:
                        BPLC_extensionCardHandler   ();
    extensionCard*      addNewExtensionCard         (const e_EXTENSION_CARD_TYPE_t EXTENSION_CARD_TYPE);
    void                tickAllextensionCards       ();
    e_BPLC_ERROR_t      getError                    (){return this->getError()};
    void                mapExtensionCardChannel     (IO_Interface* P_IO_OBJECT, const e_EXTENSION_CARD_TYPE_t CARD, const uint8_t CHANNEL);


    private:
    void            addextensionCardToList  (extensionCard* CARD_TO_ADD);
    extensionCard*  searchExtensionCard     (const e_EXTENSION_CARD_TYPE_t  SEARCHED_EXTENSION_CARD);
    extensionCard*  p_firstExtensionCard;    
};
#endif