#include "BPLC_ecManager.h"


String getEcName(e_EC_TYPE_t EC_TYPE)
{
    switch(EC_TYPE)
    {          
        case EC__MCU11revA:
            return "MCU11revA";
            break;
        case EC__MCU11revB:
            return "MCU11revB";
            break;

        case EC__MCU11revC:
            return "MCU11revC";
            break;

        case EC__AIN11revA:
            return "AIN11revA";
            break;

        case EC__DIN11revA:
            return "DIN11revA";
            break;

        case EC__DO11revA:
            return "DO11revA";
            break;

        case EC__REL11revA:
            return "REL11revA";
            break;

        case EC__MOT11revA:
            return "MOT11revA";
            break;

        case EC__TMP11revA:
            return "TMP11revA";
            break;

        case EC__PPO11revA:
            return "PPO11revA";
            break;

        case EC__NANO11revA:
            return "NANO11revA";
            break;

        case EC__FUSE11revA:
            return "FUSE11revA";
            break;

        case EC__FUSE12revA:
            return "FUSE12revA";
            break;         

            default:
            return "TEXTS NEED UPDATE";
            break;
    }
}


//Wenn Counter verwendet werden, wird ein extra Task für das Interrupthandling erzeugt. Die dinHal wird dann aus dem ecMtask gelöscht
//Erstmal für eine schnelle DinKarte ausgelegt, könnte aber ggf. erweitert werden 
static e_MCU_INT_ISR_t isrState = MCU_INT_ISR__IDLE;


//Public Interface
BPLC_extensionCardManager::BPLC_extensionCardManager()
{
    memset(this, 0, sizeof(BPLC_extensionCardManager));
    this->to_I2cScan.setInterval(10000);
    this->to_readInputs.setInterval(50);
}
void BPLC_extensionCardManager::begin()
{

}
void BPLC_extensionCardManager::mapObjectToExtensionCard(IO_Interface* P_IO_OBJECT, const e_EC_TYPE_t CARD, const e_EC_ADDR_t ADDR, const e_EC_CHANNEL_t CHANNEL)                                
{
    extensionCard*  p_cardToMapChannelTo    = this->getExtensionCard(CARD, ADDR);  
    const bool      CARD_FOUND              = (p_cardToMapChannelTo != nullptr);
 
    //Karte wurde in common ecM Liste gefunden
    if(CARD_FOUND)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);      
        if(CARD == EC__DIN11revA)
        {
            switch(P_IO_OBJECT->getIoType())
            {
                case IO_TYPE__DIGITAL_COUNTER:
                case IO_TYPE__POSITION_ENCODER:
                case IO_TYPE__RPM_SENS:
                    this->ecCardNeedRealTimeProcessing[ADDR] = true;        //Diese EC als Echtzeitfähig makieren
                break;
            }
        }
    }   
    else
    {//Error Setzen        
        this->setError(ECM_ERROR__EC_NOT_DEFINED, __FILENAME__, __LINE__);       
    }       
}  

//EC Karten Handling
bool BPLC_extensionCardManager::addNewExtensionCard(const e_EC_TYPE_t EXTENSION_CARD_TYPE, const e_EC_ADDR_t ADDR)
{
    bool newEcAdded = false;   
    const bool EC_NOT_CREATED_YET = (this->getExtensionCard(EXTENSION_CARD_TYPE, ADDR) == nullptr);
    
    if(EC_NOT_CREATED_YET)
    {
        halInterface* p_newHalInterface;
        
        switch (EXTENSION_CARD_TYPE)
        {
            case EC__MCU11revA:    
                p_newHalInterface = new HAL_MCU11_revA(&isrState);  
                break;

            case EC__MCU11revB:    
            case EC__MCU11revC://Gleiches pinning, nur änderungen im Layout 
                p_newHalInterface = new HAL_MCU11_revB(&isrState);               
                break;   

            case EC__AIN11revA:          
                p_newHalInterface = new HAL_AIN11();    
                break; 
       
            case EC__DIN11revA:              
                p_newHalInterface = new HAL_DIN11(); 
                break;                         
  
            case EC__DO11revA:        
                p_newHalInterface = new HAL_DO11();    
                break;                 

            case EC__REL11revA:                
                p_newHalInterface = new HAL_REL11();
                break;

            case EC__MOT11revA:                
                p_newHalInterface = new HAL_MOT11();                                                        
                break;
   
            case EC__TMP11revA:                
                p_newHalInterface = new HAL_TMP11();                                                                         
                break;

            case EC__PPO11revA:                
                p_newHalInterface = new HAL_PPO11();                                                                         
                break;
                
            default:
            case EC__NO_TYPE_DEFINED:
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
        extensionCard*  p_extensionCardToTick       = this->p_firstExtensionCard;
        const bool      TIME_TO_READ_INPUTS         = (!this-to_readInputs.check());
        const bool      NEW_INPUTSTATES_AVAILABLE   = (isrState != MCU_INT_ISR__IDLE);
        
        if(NEW_INPUTSTATES_AVAILABLE)
        {
            this->to_readInputs.reset();
        }
                      
        while(p_extensionCardToTick != nullptr)
        {
            halInterface*   p_halInterface  = p_extensionCardToTick->getHalInterface();
            const bool      HAL_OK          = (p_halInterface->getModuleErrorCount() == 0);            
            
            if(HAL_OK)
            {
                switch(p_extensionCardToTick->getCardType())            
                {
                    case EC__DIN11revA:        
                        if(TIME_TO_READ_INPUTS)
                        {
                            p_halInterface->tick();                     
                            isrState = MCU_INT_ISR__IDLE;
                        }                               
                    break;

                    default:
                        p_halInterface->tick();
                    break;
                }      
            }      
            p_extensionCardToTick = p_extensionCardToTick->getNext();      
        }    
    }
}

//Listen Handling
extensionCard* BPLC_extensionCardManager::getExtensionCard(const e_EC_TYPE_t SEARCHED_EXTENSION_CARD, const e_EC_ADDR_t ADDR)
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
void BPLC_extensionCardManager::deleteExtensionCardFromList(extensionCard* CARD_TO_DELETE_FROM_LIST)
{    
    if(this->p_firstExtensionCard != nullptr)
    {
        extensionCard* p_searchedCard           = this->p_firstExtensionCard;
        extensionCard* p_cardBeforeSearchedCard = this->p_firstExtensionCard;   

        while(p_searchedCard != CARD_TO_DELETE_FROM_LIST)
        {
            //Falls der nächste Port der gesuchte Port ist, pointer auf aktuellen speichern.         
            if(p_searchedCard->getNext() == CARD_TO_DELETE_FROM_LIST)
            {
                p_cardBeforeSearchedCard = p_searchedCard;
            }
            p_searchedCard = p_searchedCard->getNext();
        }
        if(p_searchedCard != nullptr)
        {
            //Falls erste Karte, dann nur nächste Karte auf den Anfang vorziehen
            if(CARD_TO_DELETE_FROM_LIST == this->p_firstExtensionCard)
            {   
                this->p_firstExtensionCard = p_searchedCard->getNext();
            }
            else
            {   //Lücke schließen      
                p_cardBeforeSearchedCard->setNext(p_searchedCard->getNext());        
            }   
                this->printLog(getEcName(CARD_TO_DELETE_FROM_LIST->getCardType()) + " ADDR:" + String(CARD_TO_DELETE_FROM_LIST->getAddr()) + " DELETED FROM ECM LIST", __FILENAME__, __LINE__);
            }
        }       
}