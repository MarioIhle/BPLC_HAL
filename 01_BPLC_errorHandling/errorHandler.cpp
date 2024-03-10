#include "BPLC_errorHandler.h"
//Public
BPLC_moduleErrorHandler::BPLC_moduleErrorHandler()
{
    this->p_firstError              = nullptr;
    this->p_superiorErrorManager    = nullptr;
    this->errorCount                = 0;
}
uint8_t BPLC_moduleErrorHandler::getErrorCount()
{
    return this->errorCount;
}  
s_error_t* BPLC_moduleErrorHandler::getError(uint8_t ERROR_NUMBER)
{
    errorListElement* p_searchedError = this->p_firstError;

    for(uint8_t errorNumber = 0; errorNumber < ERROR_NUMBER; errorNumber++)
    {        
        p_searchedError = p_searchedError->getNextError();     
    }

    return p_searchedError->getErrorData();  
}    
void BPLC_moduleErrorHandler::setError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    //Nur speichern, wenn noch nicht vorhanden
    if(this->searchError(ERROR_CODE) == nullptr)
    {
        s_error_t ERROR_DATA;
        ERROR_DATA.errorCode    = ERROR_CODE;
        ERROR_DATA.timestamp    = millis();
        ERROR_DATA.file         = FILE;
        ERROR_DATA.line         = LINE;

        //Error buffer voll, keine Error objekte mehr erzeugem, da sonst Ram überläuft
        if(this->errorCount < ERROR_BUFFER_SIZE)
        {
            errorListElement* p_newError = new errorListElement;        
            p_newError->setErrorData(ERROR_DATA);

            this->addErrorToList(p_newError);
            this->log.printError(ERROR_CODE, FILE, LINE);

            if(this->p_superiorErrorManager != nullptr)
            {
                p_superiorErrorManager->setError(ERROR_CODE, FILE, LINE);
            }
            this->errorCount++;
        }
    }
}
void BPLC_moduleErrorHandler::resetError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    errorListElement* p_errorToDelete = this->searchError(ERROR_CODE);

    if(p_errorToDelete != nullptr)
    {
        this->deleteErrorFromList(p_errorToDelete);
        this->log.printLog("reset Error: " + String(ERROR_CODE) , FILE, LINE);

        if(this->p_superiorErrorManager != nullptr)
        {
            p_superiorErrorManager->resetError(ERROR_CODE, FILE, LINE);
        }
        this->errorCount--;
    }    
}
void BPLC_moduleErrorHandler::resetAllErrors(String FILE, const uint16_t LINE)
{
    this->log.printLog("reset all module errors !", FILE, LINE);

    const uint8_t ERRORS_SET = this->getErrorCount();
    
    for(uint8_t ERROR = 0; ERROR < ERRORS_SET; ERROR++)
    {
        const e_BPLC_ERROR_t ERROR_CODE_TO_DELETE = this->getError(ERROR)->errorCode;
        this->resetError(ERROR_CODE_TO_DELETE, FILE, LINE);
    }    
    this->errorCount = 0;
}
//Private
errorListElement* BPLC_moduleErrorHandler::searchError(const e_BPLC_ERROR_t ERROR_CODE)
{
    errorListElement* p_searchedError = this->p_firstError;

    while (p_searchedError != nullptr)
    {
        if(p_searchedError->getErrorData()->errorCode == ERROR_CODE)
        {
            return p_searchedError;
        }
        p_searchedError = p_searchedError->getNextError();
    }

    return nullptr;
}
void BPLC_moduleErrorHandler::addErrorToList(errorListElement* P_ERROR_TO_ADD)
{
    if(this->p_firstError == nullptr)
    {
        this->p_firstError = P_ERROR_TO_ADD;
    }
    else
    {
        P_ERROR_TO_ADD->setNextError(this->p_firstError);
        this->p_firstError = P_ERROR_TO_ADD;
    } 
}
void BPLC_moduleErrorHandler::deleteErrorFromList(errorListElement* ERROR_TO_DELETE)
{
    errorListElement* p_serachedError             = this->p_firstError;
    errorListElement* p_errorBeforeErrorToDelete  = this->p_firstError;    

    while(p_serachedError != ERROR_TO_DELETE)
    {
        //Falls der nächste Error der gesuchte Error ist, pointer auf aktuellen speichern.         
        if(p_serachedError->getNextError() == ERROR_TO_DELETE)
        {
            p_errorBeforeErrorToDelete = p_serachedError;
        }
        p_serachedError = p_serachedError->getNextError();
    }

    if(ERROR_TO_DELETE == this->p_firstError)
    {   //Neuen ersten Error definieren
        this->p_firstError = ERROR_TO_DELETE->getNextError();
    }
    else
    {   //Lücke schließen      
        p_errorBeforeErrorToDelete->setNextError(ERROR_TO_DELETE->getNextError());        
    }   
    //Error löschen
    delete ERROR_TO_DELETE;   
}