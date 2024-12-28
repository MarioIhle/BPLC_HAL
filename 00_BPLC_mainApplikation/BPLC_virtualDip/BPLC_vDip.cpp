#include "BPLC_vDip.h"

void BPLC_vDip::setState(const bool STATE)
{
    if(STATE)
    {
        this->value = 1;
    }
    else
    {
        this->value = 0;
    }
}
bool BPLC_vDip::getState()
{
    if(this->value > 0)
    {
        this->value = 1;      
    }
    else if(this->value <= 0)
    {
        this->value = 0;
    }
    return (this->value > 0);
}

//Manager
BPLC_vDip*  BPLC_vDipManager::searchVDip(String NAME)
{
    BPLC_vDip* p_temp = this->p_first;

    while (p_temp != nullptr)
    {
        if(p_temp->getName() == NAME)
        {
            return p_temp;
        }
        p_temp = p_temp->getNext();
    }
    return nullptr;    
}
    
void BPLC_vDipManager::addNewVDipToList(BPLC_vDip* P_NEW_DIP)
{
    P_NEW_DIP->setNext(this->p_first);
    this->p_first = P_NEW_DIP;
}

BPLC_vDip* BPLC_vDipManager::createNewVirtualDip(String NAME)
{
    const bool VIRTUAL_DIP_NOT_FOUND_MUST_BE_NEW = (this->searchVDip(NAME) == nullptr);
    BPLC_vDip* newVirtualDip;

    if(VIRTUAL_DIP_NOT_FOUND_MUST_BE_NEW)
    {
        newVirtualDip = new BPLC_vDip(NAME);
        this->addNewVDipToList(newVirtualDip);
    }
    return newVirtualDip;
}