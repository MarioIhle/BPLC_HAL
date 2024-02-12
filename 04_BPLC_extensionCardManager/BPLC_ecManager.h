#ifndef BPLC_extensionCardManager_h
#define BPLC_extensionCardManager_h

#include "Arduino.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_ERRORS.h"
//HAL´s
#include "HAL_MCU11.h"
#include "HAL_DIN11.h"
#include "HAL_AIN11.h"
#include "HAL_REL11.h"
#include "HAL_DO11.h"
#include "HAL_MOT11.h"

typedef enum
{
    MCU_CARD__NO_MCU_DEFINED,

    MCU_CARD__MCU11revA,
    MCU_CARD__MCU11revB,


    EXTENSION_CARD__NO_CARD_DEFINED,

    EXTENSION_CARD__DIN11revA_1,
    EXTENSION_CARD__DIN11revA_2,
    EXTENSION_CARD__DIN11revA_3,
    EXTENSION_CARD__DIN11revA_4,

    EXTENSION_CARD__DO11revA_1,
    EXTENSION_CARD__DO11revA_2,
    EXTENSION_CARD__DO11revA_3,
    EXTENSION_CARD__DO11revA_4,

    EXTENSION_CARD__AIN11revA_1,
    EXTENSION_CARD__AIN11revA_2,
    EXTENSION_CARD__AIN11revA_3,
    EXTENSION_CARD__AIN11revA_4,

    EXTENSION_CARD__REL11revA_1,
    EXTENSION_CARD__REL11revA_2,
    EXTENSION_CARD__REL11revA_3,
    EXTENSION_CARD__REL11revA_4,

    EXTENSION_CARD__MOT11revA_1,
    EXTENSION_CARD__MOT11revA_2,
    EXTENSION_CARD__MOT11revA_3,
    EXTENSION_CARD__MOT11revA_4,

    EXTENSION_CARD__NANO11revA_1,
    EXTENSION_CARD__NANO11revA_2,
    EXTENSION_CARD__NANO11revA_3,
    EXTENSION_CARD__NANO11revA_4,
    
    EXTENSION_CARD__FUSE11revA_1,
    EXTENSION_CARD__FUSE11revA_2,
    EXTENSION_CARD__FUSE11revA_3,
    EXTENSION_CARD__FUSE11revA_4,

    EXTENSION_CARD__FUSE12revA_1,
    EXTENSION_CARD__FUSE12revA_2,
    EXTENSION_CARD__FUSE12revA_3,
    EXTENSION_CARD__FUSE12revA_4,

    EXTENSION_CARD__TYPE_COUNT,

}e_EXTENSION_CARD_TYPE_t;

class extensionCard
{    
    public:
                            extensionCard   (){this->cardType = EXTENSION_CARD__NO_CARD_DEFINED; this->p_next = nullptr; this->p_hal = nullptr;}
    void                    setNext         (extensionCard* P_NEXT){this->p_next = P_NEXT;}
    extensionCard*          getNext         (){return this->p_next;}
    halInterface*           getHalInterface (){return this->p_hal;}
    void                    setHalInterface (halInterface* P_HAL_INTERFACE){this->p_hal = P_HAL_INTERFACE;}
    void                    setCardType     (const e_EXTENSION_CARD_TYPE_t CARD_TYPE){this->cardType = CARD_TYPE;}
    e_EXTENSION_CARD_TYPE_t getCardType     (){return this->cardType;}

    private:
    extensionCard*          p_next;
    halInterface*           p_hal;
    e_EXTENSION_CARD_TYPE_t cardType; 
};

class BPLC_extensionCardHandler:BPLC_errorHandler, BPLC_LOG
{
    public:
                        BPLC_extensionCardHandler   ();    
    void                tick                        ();    
    void                mapObjectToExtensionCard    (IO_Interface* P_IO_OBJECT, const e_EXTENSION_CARD_TYPE_t CARD, const uint8_t CHANNEL);
    e_BPLC_ERROR_t      getExtensionCradError       (){return this->getError();}
    


    private:    
    extensionCard*  addNewExtensionCard             (const e_EXTENSION_CARD_TYPE_t EXTENSION_CARD_TYPE);
    void            addExtensionCardToList          (extensionCard* CARD_TO_ADD);
    extensionCard*  searchExtensionCard             (const e_EXTENSION_CARD_TYPE_t  SEARCHED_EXTENSION_CARD);
    extensionCard*  p_firstExtensionCard;    
    counter         isrCounter;
};
#endif