#ifndef BPLC_extensionCardManager_h
#define BPLC_extensionCardManager_h

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

#include <esp_task_wdt.h>
typedef enum
{
    EC__NO_TYPE_DEFINED,

    EC__MCU11revA,
    EC__MCU11revB,
    EC__MCU11revC,   

    EC__AIN11revA,
    EC__DIN11revA,
    EC__DO11revA, 
    EC__REL11revA,
    EC__MOT11revA,
    EC__TMP11revA,
    EC__PPO11revA,
    EC__NANO11revA,  
    EC__FUSE11revA,  
    EC__FUSE12revA,

    EC__TYPE_COUNT,

}e_EC_TYPE_t;


class extensionCard
{    
    public:
                            extensionCard   (){this->cardType = EC__NO_TYPE_DEFINED; this->addr = EC_ADDR_NOT_DEFINED;  this->p_next = nullptr; this->p_hal = nullptr;}
    void                    setNext         (extensionCard* P_NEXT){this->p_next = P_NEXT;}
    extensionCard*          getNext         (){return this->p_next;}
    halInterface*           getHalInterface (){return this->p_hal;}
    void                    setHalInterface (halInterface* P_HAL_INTERFACE){this->p_hal = P_HAL_INTERFACE;}
    void                    setCardType     (const e_EC_TYPE_t CARD_TYPE){this->cardType = CARD_TYPE;}
    e_EC_TYPE_t             getCardType     (){return this->cardType;}
    void                    setAddr         (const e_EC_ADDR_t ADDR){this->addr = ADDR;}
    e_EC_ADDR_t             getAddr         (){return this->addr;}


    private:
    extensionCard*          p_next;
    halInterface*           p_hal;
    e_EC_TYPE_t             cardType; 
    e_EC_ADDR_t             addr;
};


class BPLC_extensionCardManager: public BPLC_moduleErrorInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint
{
    public:
                        BPLC_extensionCardManager           ();    
    void                tick                                ();    
    void                mapObjectToExtensionCard            (IO_Interface* P_IO_OBJECT, const e_EC_TYPE_t CARD, const e_EC_ADDR_t ADDR, const e_EC_CHANNEL_t CHANNEL);
    bool                addNewExtensionCard                 (const e_EC_TYPE_t EXTENSION_CARD_TYPE, const e_EC_ADDR_t ADDR);
    //Modul Error Interface   
    uint8_t             getModuleErrorCount                 ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t      getModuleErrorCode                  (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void                resetAllModuleErrors                (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void                setSuperiorErrorManager             (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}


    private:        
    //Bus Scan
    void                scanForUnkonwnI2CDevices            ();
    bool                scanCardAddresses                   (const e_EC_TYPE_t CARD_TYPE, const uint8_t* P_ADDRESSES_TO_SCAN, const uint8_t ADDRESS_COUNT);
    Timeout             to_I2cScan;
    //ExtensionCard List
    extensionCard*      p_firstExtensionCard;    
    void                addExtensionCardToList              (extensionCard* CARD_TO_ADD);
    extensionCard*      searchExtensionCard                 (const e_EC_TYPE_t  SEARCHED_EXTENSION_CARD, const e_EC_ADDR_t ADDR);
    //Input Interrupt count
    volatile uint64_t   intIsrOccoured;
    Timeout             to_readInputs;
};
#endif