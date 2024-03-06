#ifndef BPLC_extensionCardManager_h
#define BPLC_extensionCardManager_h

#include "Arduino.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_ERROR.h"
#include "BPLC_PLI.h"
//HAL´s
#include "HAL_MCU11.h"
#include "HAL_AIN11.h"
#include "HAL_DIN11.h"
#include "HAL_REL11.h"
#include "HAL_DO11.h"
#include "HAL_REL11.h"
#include "HAL_MOT11.h"
#include "HAL_TMP11.h"
#include "HAL_PPO11.h"
#include "HAL_NANO11.h"
#include "HAL_FUSE12.h"


typedef enum
{
    BPLC_CARD__NO_CARD_DEFINED,

    BPLC_CARD__MCU11revA,
    BPLC_CARD__MCU11revB,
    BPLC_CARD__MCU11revC,   

    BPLC_CARD__AIN11revA,
    BPLC_CARD__DIN11revA,
    BPLC_CARD__DO11revA, 
    BPLC_CARD__REL11revA,
    BPLC_CARD__MOT11revA,
    BPLC_CARD__TMP11revA,
    BPLC_CARD__PPO11revA,
    BPLC_CARD__NANO11revA,  
    BPLC_CARD__FUSE11revA,  
    BPLC_CARD__FUSE12revA,

    BPLC_CARD__TYPE_COUNT,

}e_BPLC_CARD_TYPE_t;


class extensionCard
{    
    public:
                            extensionCard   (){this->cardType = BPLC_CARD__NO_CARD_DEFINED; this->addr = EC_ADDR_NOT_DEFINED;  this->p_next = nullptr; this->p_hal = nullptr;}
    void                    setNext         (extensionCard* P_NEXT){this->p_next = P_NEXT;}
    extensionCard*          getNext         (){return this->p_next;}
    halInterface*           getHalInterface (){return this->p_hal;}
    void                    setHalInterface (halInterface* P_HAL_INTERFACE){this->p_hal = P_HAL_INTERFACE;}
    void                    setCardType     (const e_BPLC_CARD_TYPE_t CARD_TYPE){this->cardType = CARD_TYPE;}
    e_BPLC_CARD_TYPE_t      getCardType     (){return this->cardType;}
    void                    setAddr         (const e_EC_ADDR_t ADDR){this->addr = ADDR;}
    e_EC_ADDR_t             getAddr         (){return this->addr;}


    private:
    extensionCard*          p_next;
    halInterface*           p_hal;
    e_BPLC_CARD_TYPE_t      cardType; 
    e_EC_ADDR_t             addr;
};

class BPLC_extensionCardManager:BPLC_errorHandler, BPLC_LOG
{
    public:
                        BPLC_extensionCardManager           ();    
    void                tick                                ();    
    void                mapObjectToExtensionCard            (IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const e_EC_ADDR_t ADDR, const uint8_t CHANNEL);
    e_BPLC_ERROR_t      getModulError                       (){return this->getError();}
    bool                i2cAddressIsUsedByExtensionCard     (const uint8_t I2C_ADDRESS);
    bool                addNewExtensionCard                 (const e_BPLC_CARD_TYPE_t EXTENSION_CARD_TYPE, const e_EC_ADDR_t ADDR);


    private:        
    void                addExtensionCardToList              (extensionCard* CARD_TO_ADD);
    extensionCard*      searchExtensionCard                 (const e_BPLC_CARD_TYPE_t  SEARCHED_EXTENSION_CARD, const e_EC_ADDR_t ADDR);
    extensionCard*      p_firstExtensionCard;    
    volatile uint64_t   isrCount;
};
#endif