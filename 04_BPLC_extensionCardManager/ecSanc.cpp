#include "BPLC_ecManager.h"

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
        const bool EC_IS_DEFINED = this->getExtensionCard(CARD_TYPE, (e_EC_ADDR_t)CARD_ADDR);
        
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