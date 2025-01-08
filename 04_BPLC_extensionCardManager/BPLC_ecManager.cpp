#include "BPLC_ecManager.h"

BPLC_extensionCardManager* p_ECM;

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

void ecmTask(void* arg)
{
    esp_task_wdt_init(1, true);                
    esp_task_wdt_add(NULL);   

    while(true)
    {
        esp_task_wdt_reset();
        p_ECM->tick();
        delay(1);
    }
}

//Wenn Counter verwendet werden, wird ein extra Task für das Interrupthandling erzeugt. Die dinHal wird dann aus dem ecMtask gelöscht
//Erstmal für eine schnelle DinKarte ausgelegt, könnte aber ggf. erweitert werden 
extensionCard*      p_firstInterruptEc = nullptr;
e_MCU_INT_ISR_t     isrState = MCU_INT_ISR__IDLE;


void din11TaskForCounters(void* arg)
{//I2C limit ist ca. 1,5k Aufrufe pro secunde, reicht für 100k U/min
    disableCore0WDT();
    esp_task_wdt_init(1, true);                
    esp_task_wdt_add(NULL);                

    while(1)
    {     
        esp_task_wdt_reset();   
        extensionCard* p_ecToTick = p_firstInterruptEc;

        if(isrState == MCU_INT_ISR__NEW_INT)
        {              
            while(p_ecToTick != nullptr)
            {
                p_ecToTick->getHalInterface()->tick();
                p_ecToTick = p_ecToTick->getNext();
            }   
            isrState = MCU_INT_ISR__WAIT_FOR_ECM;
        }                
    }
}

//Public Interface
BPLC_extensionCardManager::BPLC_extensionCardManager()
{
    this->intIsrOccoured = 10;            //Falls DIN EC vorhanden, wird im ersten tick das Flag erkannt und einmal alle Karten gelesen
    this->to_I2cScan.setInterval(10000);
    this->to_readInputs.setInterval(100);
}
void BPLC_extensionCardManager::begin()
{
    p_ECM = this;
    xTaskCreatePinnedToCore(ecmTask, "ecmTask", 4096, NULL, 1, NULL, 0);
}
void BPLC_extensionCardManager::mapObjectToExtensionCard(IO_Interface* P_IO_OBJECT, const e_EC_TYPE_t CARD, const e_EC_ADDR_t ADDR, const e_EC_CHANNEL_t CHANNEL)                                
{
    extensionCard*  p_cardToMapChannelTo            = this->searchExtensionCard(CARD, ADDR);  
    const bool      CARD_FOUND_IN_COMON_ECM_LIST    = (p_cardToMapChannelTo != nullptr);
 
    //Karte wurde in common ecM Liste gefunden
    if(CARD_FOUND_IN_COMON_ECM_LIST)
    {
        p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);        
    }
    //Gesuchte Karte möglicherweise eine DIN11 vom Interrupt handler?
    else if(CARD == EC__DIN11revA)
    {      
        p_cardToMapChannelTo = p_firstInterruptEc;

        while (p_cardToMapChannelTo != nullptr)
        {//Richtige Karte finden
            if(p_cardToMapChannelTo->getAddr() == ADDR)
            {
                p_cardToMapChannelTo->getHalInterface()->mapObjectToChannel(P_IO_OBJECT, CHANNEL);
                break;
            }
            p_cardToMapChannelTo = p_cardToMapChannelTo->getNext();
        }        
    }
    else
    {//Error Setzen        
        this->setError(ECM_ERROR__EC_NOT_DEFINED, __FILENAME__, __LINE__);       
    }    

    if(CARD == EC__DIN11revA)
    {           
        //externen dinTask erzeugen und aus ecM Liste löschen, wenn diese IO_objkete verwedet werden
        switch(P_IO_OBJECT->getIoType())
        {        
            case IO_TYPE__DIGITAL_COUNTER:
            case IO_TYPE__POSITION_ENCODER:
            case IO_TYPE__ROTARY_ENCODER:
            case IO_TYPE__RPM_SENS:
                {
                    //Es sollte grundsätzlich das extensionCard objekt beim Start des Systems erzeugt worden sein, kann also einfach kopiert und anschließend aus der liste gelöscht werden
                    const bool TASK_NEEDS_TO_BE_CREATED = CARD_FOUND_IN_COMON_ECM_LIST;
                                    
                    if(TASK_NEEDS_TO_BE_CREATED)
                    {    
                        if(p_firstInterruptEc == nullptr)
                        {
                            p_cardToMapChannelTo->setNext(nullptr);
                            p_firstInterruptEc = p_cardToMapChannelTo;                       
                             
                            xTaskCreatePinnedToCore(din11TaskForCounters, "dinHalTask", 4096, NULL, 1, NULL, 0);  //Task erstellen wenn es die erste EC ist    
                            this->printLog("DIN11 TASK ERZEUGT", __FILENAME__, __LINE__);                            
                        }
                        else
                        {
                            p_cardToMapChannelTo->setNext(p_firstInterruptEc);
                            p_firstInterruptEc = p_cardToMapChannelTo;                
                        }
                        this->deleteExtensionCardFromList(p_cardToMapChannelTo);  //Kartenobjekt von ecM Liste löschen      
                    }
                }
                break;

            default:
                break;
        } 
    }        
}  

//EC Karten Handling
bool BPLC_extensionCardManager::addNewExtensionCard(const e_EC_TYPE_t EXTENSION_CARD_TYPE, const e_EC_ADDR_t ADDR)
{
    bool newEcAdded = false;   
    if(this->searchExtensionCard(EXTENSION_CARD_TYPE, ADDR) == nullptr)
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
        extensionCard* p_extensionCardToTick = this->p_firstExtensionCard;

        const bool TIME_TO_READ_INPUTS        = this-to_readInputs.checkAndReset();
        const bool NEW_INPUTSTATES_AVAILABLE  = (isrState != MCU_INT_ISR__IDLE);
        
        if(NEW_INPUTSTATES_AVAILABLE && TIME_TO_READ_INPUTS)
        {
            isrState = MCU_INT_ISR__IDLE;       
        }
            
        while(p_extensionCardToTick != nullptr)
        {
            const bool HAL_SUCCESSFUL_INITIALIZED = (p_extensionCardToTick->getHalInterface()->getModuleErrorCount() == 0);
            
            if(HAL_SUCCESSFUL_INITIALIZED)
            {
                switch(p_extensionCardToTick->getCardType())            
                {
                    case EC__DIN11revA:                       
                        if(NEW_INPUTSTATES_AVAILABLE && TIME_TO_READ_INPUTS)
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
    }
    if(this->to_I2cScan.checkAndReset())
    {
        //this->scanForUnkonwnI2CDevices();
    }      
}
//I2C Scan
void BPLC_extensionCardManager::scanForUnkonwnI2CDevices()
{         
    //AIN11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_AIN11revA = this->scanCardAddresses(EC__AIN11revA, &AIN11_I2C_ADDRESSES[0], AIN11_ADDRESS_COUNT);
    
    if(THERE_IS_A_UNDEFINED_AIN11revA)
    {
        this->setError(AIN11_ERROR__UNDEFINED_AIN11_FOUND, __FILENAME__, __LINE__);
    }
    //DIN11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_DIN11revA = this->scanCardAddresses(EC__DIN11revA, &DIN11_I2C_ADDRESSES[0], DIN11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_DIN11revA)
    {
        this->setError(DIN11_ERROR__UNDEFINED_DIN11_FOUND, __FILENAME__, __LINE__);
    }
    //DO11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_DO11revA = this->scanCardAddresses(EC__DO11revA, &DO11_I2C_ADDRESSES[0], DO11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_DO11revA)
    {
        this->setError(DO11_ERROR__UNDEFINED_DO11_FOUND, __FILENAME__, __LINE__);
    }
    //REL11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_REL11revA = this->scanCardAddresses(EC__REL11revA, &REL11_I2C_ADDRESSES[0], REL11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_REL11revA)
    {
        this->setError(REL11_ERROR__UNDEFINED_REL11_FOUND, __FILENAME__, __LINE__);
    }    
    //MOT11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_MOT11revA = this->scanCardAddresses(EC__MOT11revA, &MOT11_I2C_ADDRESSES[0], MOT11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_MOT11revA)
    {
        this->setError(MOT11_ERROR__UNDEFINED_MOT11_FOUND, __FILENAME__, __LINE__);
    }
    //TMP11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_TMP11revA = this->scanCardAddresses(EC__TMP11revA, &TMP11_I2C_ADDRESSES[0], TMP11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_TMP11revA)
    {
        this->setError(TMP11_ERROR__UNDEFINED_TMP11_FOUND, __FILENAME__, __LINE__);
    }   
    //PPO11revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_PPO11revA = this->scanCardAddresses(EC__PPO11revA, &PPO11_I2C_ADDRESSES[0], PPO11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_PPO11revA)
    {
        this->setError(PPO11_ERROR__UNDEFINED_PPO11_FOUND, __FILENAME__, __LINE__);
    }
    //NANOrevA Cards suchen
    const bool THERE_IS_A_UNDEFINED_NANOrevA = this->scanCardAddresses(EC__NANO11revA, &NANO11_I2C_ADDRESSES[0], NANO11_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_NANOrevA)
    {
        this->setError(NANO11_ERROR__UNDEFINED_NANO11_FOUND, __FILENAME__, __LINE__);
    }
    //FUSE12revA Cards suchen
    const bool THERE_IS_A_UNDEFINED_FUSE12revA = this->scanCardAddresses(EC__FUSE12revA, &FUSE12_I2C_ADDRESSES[0], FUSE12_ADDRESS_COUNT);

    if(THERE_IS_A_UNDEFINED_FUSE12revA)
    {
        this->setError(FUSE12_ERROR__UNDEFINED_FUSE12_FOUND, __FILENAME__, __LINE__);
    }    
}
bool BPLC_extensionCardManager::scanCardAddresses(const e_EC_TYPE_t CARD_TYPE, const uint8_t* P_ADDRESSES_TO_SCAN, const uint8_t ADDRESS_COUNT)
{
    bool unkownCardFound = false;

    for(uint8_t CARD_ADDR = 0; CARD_ADDR < ADDRESS_COUNT; CARD_ADDR++)
    {
        const uint8_t EC_I2C_ADDRESS = P_ADDRESSES_TO_SCAN[CARD_ADDR];
        //Prüfen ob Karte normal in Nutzung und Definiert
        const bool EC_IS_DEFINED = this->searchExtensionCard(CARD_TYPE, (e_EC_ADDR_t)CARD_ADDR);
        
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
                this->printLog("UNDEFINED I2C ADDRESS: " + String(EC_I2C_ADDRESS), __FILENAME__, __LINE__);
                break;
            }            
        }
    }
    return unkownCardFound;
}
//Listen Handling
extensionCard* BPLC_extensionCardManager::searchExtensionCard(const e_EC_TYPE_t SEARCHED_EXTENSION_CARD, const e_EC_ADDR_t ADDR)
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