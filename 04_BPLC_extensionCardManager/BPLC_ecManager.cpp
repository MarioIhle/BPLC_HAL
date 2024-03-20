#include "BPLC_ecManager.h"
//Public Interface
BPLC_extensionCardManager::BPLC_extensionCardManager()
{
    this->isrCount = 0;
    this->to_I2cScan.setInterval(10000);
}
void BPLC_extensionCardManager::mapObjectToExtensionCard(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const e_EC_ADDR_t ADDR, const uint8_t CHANNEL)                                
{
    extensionCard* p_cardToMapChannelTo = this->searchExtensionCard(CARD, ADDR);

    if(p_cardToMapChannelTo != nullptr)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
    }
    else
    {//Error Setzen        
        this->setError(ECM_ERROR__EC_NOT_DEFINED, __FILENAME__, __LINE__);       
    }      
}  
bool BPLC_extensionCardManager::addNewExtensionCard(const e_BPLC_CARD_TYPE_t EXTENSION_CARD_TYPE, const e_EC_ADDR_t ADDR)
{
    bool newEcAdded = false;   
    if(this->searchExtensionCard(EXTENSION_CARD_TYPE, ADDR) == nullptr)
    {
        halInterface* p_newHalInterface;
        
        switch (EXTENSION_CARD_TYPE)
        {
            case BPLC_CARD__MCU11revA:    
                p_newHalInterface = new HAL_MCU11_revA(&this->isrCount);  
                break;

            case BPLC_CARD__MCU11revB:    
            case BPLC_CARD__MCU11revC://Gleiches pinning, nur änderungen im Layout 
                p_newHalInterface = new HAL_MCU11_revB(&this->isrCount);      
                break;   

            case BPLC_CARD__AIN11revA:          
                p_newHalInterface = new HAL_AIN11();    
                break; 
       
            case BPLC_CARD__DIN11revA:              
                p_newHalInterface = new HAL_DIN11();
                break;                         
  
            case BPLC_CARD__DO11revA:        
                p_newHalInterface = new HAL_DO11();    
                break;                 

            case BPLC_CARD__REL11revA:                
                p_newHalInterface = new HAL_REL11();
                break;

            case BPLC_CARD__MOT11revA:                
                p_newHalInterface = new HAL_MOT11();                                                        
                break;
   
            case BPLC_CARD__TMP11revA:                
                p_newHalInterface = new HAL_TMP11();                                                                         
                break;
                
            default:
            case BPLC_CARD__NO_CARD_DEFINED:
                this->printResetReason("CARD NOT KNOWN", __FILENAME__, __LINE__);
                abort();
                break;
        }         
        
        //System Error Manager an Hal moduleErrorManager übergeben
        p_newHalInterface->setSuperiorErrorManager(this->p_superiorErrorManager);
        //Hal mit entsprechender ADDR(wird in Hal zu entsprechender i2c addresse gemappt)initialisieren
        p_newHalInterface->init(ADDR);          
        const bool HAL_SUCCESSFUL_INITIALIZED = (bool)(p_newHalInterface->getModuleErrorCount() == 0);

        if(HAL_SUCCESSFUL_INITIALIZED)
        {            
            //Neues extensionCard Objekt erzeugen und in Liste aufnehmen
            extensionCard* p_extensionCard = new extensionCard();
            //Interface für neu erzeugte Hal an extensionCard objekt übergeben
            p_extensionCard->setHalInterface(p_newHalInterface);
            p_extensionCard->setCardType(EXTENSION_CARD_TYPE);  
            p_extensionCard->setAddr(ADDR); 
            this->addExtensionCardToList(p_extensionCard);  
            newEcAdded = true;
        }        
    }
    else
    {
        //this->printLog("EC ALREADY DEFINDED", __FILENAME__, __LINE__);
    }    
    return newEcAdded;
}
void BPLC_extensionCardManager::tick()
{
    if(this->p_firstExtensionCard!= nullptr)
    {
        extensionCard* p_extensionCardToTick = this->p_firstExtensionCard;
    
        while(p_extensionCardToTick != nullptr)
        {
            const bool HAL_SUCCESSFUL_INITIALIZED = (bool)(p_extensionCardToTick->getHalInterface()->getModuleErrorCount() == 0);

            if(HAL_SUCCESSFUL_INITIALIZED)
            {
                switch(p_extensionCardToTick->getCardType())            
                {
                    case BPLC_CARD__DIN11revA:
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
            p_extensionCardToTick = p_extensionCardToTick->getNext();      
        }
        //nach lesen ALLER (DIN) Karten count verringern
        if(this->isrCount > 0)
        {                  
            this->isrCount--;
        }  
    }
    if(this->to_I2cScan.checkAndReset())
    {
        this->scanForUnkonwnI2CDevices();
    }      
}
//I2C Scan
void BPLC_extensionCardManager::scanForUnkonwnI2CDevices()
{         
    //AIN11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_AIN11revA = this->scanCardAddresses(&AIN11_I2C_ADDRESSES[0], AIN11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_AIN11revA)
    {
        this->setError(AIN11_ERROR__UNDEFINED_AIN11_FOUND, __FILENAME__, __LINE__);
    }
    //DIN11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_DIN11revA = this->scanCardAddresses(&DIN11_I2C_ADDRESSES[0], DIN11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_DIN11revA)
    {
        this->setError(DIN11_ERROR__UNDEFINED_DIN11_FOUND, __FILENAME__, __LINE__);
    }
    //DO11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_DO11revA = this->scanCardAddresses(&DO11_I2C_ADDRESSES[0], DO11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_DO11revA)
    {
        this->setError(DO11_ERROR__UNDEFINED_DO11_FOUND, __FILENAME__, __LINE__);
    }
    //REL11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_REL11revA = this->scanCardAddresses(&REL11_I2C_ADDRESSES[0], REL11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_REL11revA)
    {
        this->setError(REL11_ERROR__UNDEFINED_REL11_FOUND, __FILENAME__, __LINE__);
    }    
    //MOT11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_MOT11revA = this->scanCardAddresses(&MOT11_I2C_ADDRESSES[0], MOT11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_MOT11revA)
    {
        this->setError(MOT11_ERROR__UNDEFINED_MOT11_FOUND, __FILENAME__, __LINE__);
    }
    //TMP11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_TMP11revA = this->scanCardAddresses(&TMP11_I2C_ADDRESSES[0], TMP11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_TMP11revA)
    {
        this->setError(TMP11_ERROR__UNDEFINED_TMP11_FOUND, __FILENAME__, __LINE__);
    }   
    //PPO11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_PPO11revA = this->scanCardAddresses(&PPO11_I2C_ADDRESSES[0], PPO11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_PPO11revA)
    {
        this->setError(PPO11_ERROR__UNDEFINED_PPO11_FOUND, __FILENAME__, __LINE__);
    }
    //NANOrevA Cards suchen
    const bool THERE_IS_A_UNDEFINED_NANOrevA = this->scanCardAddresses(&NANO11_I2C_ADDRESSES[0], NANO11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_NANOrevA)
    {
        this->setError(NANO11_ERROR__UNDEFINED_NANO11_FOUND, __FILENAME__, __LINE__);
    }
    //FUSE12revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_FUSE12revA = this->scanCardAddresses(&FUSE12_I2C_ADDRESSES[0], FUSE12_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_FUSE12revA)
    {
        this->setError(FUSE12_ERROR__UNDEFINED_FUSE12_FOUND, __FILENAME__, __LINE__);
    }
    
}
bool BPLC_extensionCardManager::scanCardAddresses(const uint8_t* P_ADDRESSES_TO_SCAN, const uint8_t ADDRESS_COUNT)
{
    bool unkownCardFound = false;

    for(uint8_t CARD_ADDR = 0; CARD_ADDR < ADDRESS_COUNT; CARD_ADDR++)
    {
        const uint8_t EC_I2C_ADDRESS = P_ADDRESSES_TO_SCAN[CARD_ADDR];
        //Prüfen ob Karte normal in Nutzung und Definiert
        const bool EC_IS_DEFINED = this->searchExtensionCard(BPLC_CARD__AIN11revA, (e_EC_ADDR_t)CARD_ADDR);
        
        if(EC_IS_DEFINED)
        {//KeepAlive wird von Hal ausgeführt, hier wird nur nach undefinierten Karten gesucht
            continue;
        }
        else
        {       
            Wire.beginTransmission(EC_I2C_ADDRESS);
            const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0); 
            //Undefinierte EC gefunden
            if(DEVICE_FOUND)
            {
                unkownCardFound = true;
            }            
        }
    }
    return unkownCardFound;
}
//Listen Handling
extensionCard* BPLC_extensionCardManager::searchExtensionCard(const e_BPLC_CARD_TYPE_t SEARCHED_EXTENSION_CARD, const e_EC_ADDR_t ADDR)
{
    extensionCard* p_searchedCard = this->p_firstExtensionCard;

    while (p_searchedCard != nullptr)
    {
        if(p_searchedCard->getCardType() == SEARCHED_EXTENSION_CARD && p_searchedCard->getAddr() == ADDR)
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