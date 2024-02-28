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
#include "HAL_TMP11.h"
#include "HAL_NANO11.h"
#include "HAL_FUSE12.h"

typedef enum
{
    BPLC_CARD__NO_CARD_DEFINED,

    BPLC_CARD__MCU11revA,
    BPLC_CARD__MCU11revB,
    BPLC_CARD__MCU11revC,   

    BPLC_CARD__DIN11revA_1 = DIN11_CARD_1_ADDRESS,
    BPLC_CARD__DIN11revA_2 = DIN11_CARD_2_ADDRESS,
    BPLC_CARD__DIN11revA_3 = DIN11_CARD_3_ADDRESS,
    BPLC_CARD__DIN11revA_4 = DIN11_CARD_4_ADDRESS,

    BPLC_CARD__DO11revA_1 = DO11_CARD_1_ADDRESS,
    BPLC_CARD__DO11revA_2 = DO11_CARD_2_ADDRESS,
    BPLC_CARD__DO11revA_3 = DO11_CARD_3_ADDRESS,
    BPLC_CARD__DO11revA_4 = DO11_CARD_4_ADDRESS,

    BPLC_CARD__AIN11revA_1 = AIN11_CARD_1_ADDRESS,
    BPLC_CARD__AIN11revA_2 = AIN11_CARD_2_ADDRESS,
    BPLC_CARD__AIN11revA_3 = AIN11_CARD_3_ADDRESS,
    BPLC_CARD__AIN11revA_4 = AIN11_CARD_4_ADDRESS,

    BPLC_CARD__REL11revA_1 = REL11_CARD_1_ADDRESS,
    BPLC_CARD__REL11revA_2 = REL11_CARD_2_ADDRESS,
    BPLC_CARD__REL11revA_3 = REL11_CARD_3_ADDRESS,
    BPLC_CARD__REL11revA_4 = REL11_CARD_4_ADDRESS,

    BPLC_CARD__MOT11revA_1 = MOT11_CARD_1_ADDRESS,
    BPLC_CARD__MOT11revA_2 = MOT11_CARD_2_ADDRESS,
    BPLC_CARD__MOT11revA_3 = MOT11_CARD_3_ADDRESS,
    BPLC_CARD__MOT11revA_4 = MOT11_CARD_4_ADDRESS,

    BPLC_CARD__NANO11revA_1 = NANO11_CARD_1_ADDRESS,
    BPLC_CARD__NANO11revA_2 = NANO11_CARD_2_ADDRESS,
    BPLC_CARD__NANO11revA_3 = NANO11_CARD_3_ADDRESS,
    BPLC_CARD__NANO11revA_4 = NANO11_CARD_4_ADDRESS,
    
    BPLC_CARD__FUSE12revA_1 = FUSE12_CARD_1_ADDRESS,
    BPLC_CARD__FUSE12revA_2 = FUSE12_CARD_2_ADDRESS,
    BPLC_CARD__FUSE12revA_3 = FUSE12_CARD_3_ADDRESS,
    BPLC_CARD__FUSE12revA_4 = FUSE12_CARD_4_ADDRESS,

    BPLC_CARD__TMP11revA_1 = TMP11_CARD_1_ADDRESS,
    BPLC_CARD__TMP11revA_2 = TMP11_CARD_2_ADDRESS,
    BPLC_CARD__TMP11revA_3 = TMP11_CARD_3_ADDRESS,
    BPLC_CARD__TMP11revA_4 = TMP11_CARD_4_ADDRESS,

    BPLC_CARD__TYPE_COUNT,

}e_BPLC_CARD_TYPE_t;

class extensionCard
{    
    public:
                            extensionCard   (){this->cardType = BPLC_CARD__NO_CARD_DEFINED; this->p_next = nullptr; this->p_hal = nullptr;}
    void                    setNext         (extensionCard* P_NEXT){this->p_next = P_NEXT;}
    extensionCard*          getNext         (){return this->p_next;}
    halInterface*           getHalInterface (){return this->p_hal;}
    void                    setHalInterface (halInterface* P_HAL_INTERFACE){this->p_hal = P_HAL_INTERFACE;}
    void                    setCardType     (const e_BPLC_CARD_TYPE_t CARD_TYPE){this->cardType = CARD_TYPE;}
    e_BPLC_CARD_TYPE_t      getCardType     (){return this->cardType;}

    private:
    extensionCard*          p_next;
    halInterface*           p_hal;
    e_BPLC_CARD_TYPE_t      cardType; 
};

class BPLC_extensionCardHandler:BPLC_errorHandler, BPLC_LOG
{
    public:
                        BPLC_extensionCardHandler           ();    
    void                tick                                ();    
    void                mapObjectToExtensionCard            (IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const uint8_t CHANNEL);
    e_BPLC_ERROR_t      getExtensionCradError               (){return this->getError();}
    bool                i2cAddressIsUsedByExtensionCard     (const uint8_t I2C_ADDRESS);
    

    private:    
    extensionCard*      addNewExtensionCard             (const e_BPLC_CARD_TYPE_t EXTENSION_CARD_TYPE);
    void                addExtensionCardToList          (extensionCard* CARD_TO_ADD);
    extensionCard*      searchExtensionCard             (const e_BPLC_CARD_TYPE_t  SEARCHED_EXTENSION_CARD);
    extensionCard*      p_firstExtensionCard;    
    volatile uint64_t   isrCount;
};
#endif