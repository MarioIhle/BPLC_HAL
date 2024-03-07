#include "BPLC_errorHandler.h"

BPLC_moduleErrorHandler::BPLC_moduleErrorHandler()
{
    this->p_firstError = nullptr;
}
uint8_t BPLC_moduleErrorHandler::getErrorCount()
{
    uint8_t     errorCount      = 0;
    BPLC_ERROR* p_searchedError = this->p_firstError;

    while (p_searchedError != nullptr)
    {
        p_searchedError = p_searchedError->getNext();
        errorCount++;
    }
    return errorCount;
}  
s_errorBufferElement_t* BPLC_moduleErrorHandler::getError(uint8_t ERROR_NUMBER)
{
    BPLC_ERROR* p_searchedError = this->p_firstError;
    for(uint8_t errorNumber = 0; errorNumber < ERROR_NUMBER; errorNumber++)
    {        
        p_searchedError = p_searchedError->getNext();     
    }
    return p_searchedError;  
}    
void BPLC_moduleErrorHandler::setError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    //Nur speichern, wenn noch nicht vorhanden
    if(this->searchError(ERROR_CODE) == nullptr)
    {
        s_errorBufferElement_t ERROR_DATA;
        ERROR_DATA.errorCode    = ERROR_CODE;
        ERROR_DATA.timestamp    = millis()
        ERROR_DATA.file         = FILE;
        ERROR_DATA.line         = LINE;

        BPLC_ERROR* p_newError = new BPLC_ERROR;        
        p_newError->setErrorData(ERROR_DATA);

        this->addErrorToList(p_newError);
        this->log.printError(ERROR_CODE, FILE, LINE);
    }
}
void BPLC_moduleErrorHandler::resetError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    BPLC_ERROR* p_errorToDelete = this->searchError(ERROR_CODE);

    if(p_errorToDelete != nullptr)
    {
        this->deleteErrorFromList(p_errorToDelete);
        this->log.printLog("reset Error: " + String(ERROR_CODE) , FILE, LINE);
    }    
}
BPLC_ERROR* BPLC_moduleErrorHandler::searchError(const e_BPLC_ERROR_t ERROR_CODE)
{
    BPLC_ERROR* p_searchedError = this->p_firstError;

    while (p_searchedError != nullptr)
    {
        if(p_searchedError->getErrorData().errorCode == ERROR_CODE)
        {
            return p_searchedError;
        }
        p_searchedError = p_searchedError->getNext();
    }

    return nullptr;
}
void BPLC_moduleErrorHandler::addErrorToList(BPLC_ERROR* P_ERROR_TO_ADD)
{
    if(this->p_firstError == nullptr)
    {
        this->p_firstError = P_ERROR_TO_ADD;
    }
    else
    {
        P_ERROR_TO_ADD->setNext(this->p_firstError);
        this->p_firstError = P_ERROR_TO_ADD;
    } 
}
void BPLC_moduleErrorHandler::deleteErrorFromList(BPLC_ERROR* ERROR_TO_DELETE)
{
    BPLC_ERROR* p_serachedError             = this->p_firstError;
    BPLC_ERROR* p_errorBeforeErrorToDelete  = this->p_firstError;    

    while(p_serachedError != ERROR_TO_DELETE)
    {
        //Falls der nächste Error der gesuchte Error ist, pointer auf aktuellen speichern.         
        if(p_serachedError->getNext() == ERROR_TO_DELETE)
        {
            p_errorBeforeErrorToDelete = p_serachedError;
        }
        p_serachedError = p_serachedError->getNext();
    }

    if(ERROR_TO_DELETE == this->p_firstError)
    {   //Neuen ersten Error definieren
        this->p_firstError = ERROR_TO_DELETE->getNext();
    }
    else
    {   //Lücke schließen      
        p_errorBeforeErrorToDelete->setNextPort(ERROR_TO_DELETE->getNextPort());        
    }   
    //Error löschen
    delete ERROR_TO_DELETE;   
}