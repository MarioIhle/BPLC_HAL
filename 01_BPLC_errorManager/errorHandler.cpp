#include "BPLC_errorHandler.h"
//Public
BPLC_moduleErrorHandler::BPLC_moduleErrorHandler()
{
    this->p_firstError              = nullptr;
    this->p_superiorErrorManager    = nullptr;
    this->errorCount                = 0;
    this->enabled                   = true;
}
uint8_t BPLC_moduleErrorHandler::getErrorCount()
{
    if(this->enabled)
    {
        return this->errorCount;
    }
    else
    {
        return 0;
    }    
}  
s_error_t* BPLC_moduleErrorHandler::getError(uint8_t ERROR_NUMBER)
{
    errorListElement* p_searchedError = this->p_firstError;

    if(this->p_firstError != nullptr)
    {

        if(ERROR_NUMBER < this->errorCount)
        {
            for(uint8_t errorNumber = 0; errorNumber < ERROR_NUMBER; errorNumber++)
            {          
                p_searchedError = p_searchedError->getNextError();     
            }
        }
    }
    else
    {//Fiktive ErrorObjekt erzeugen
        s_error_t errorData;
        errorData.errorCode = BPLC_ERROR__NO_ERROR;
        errorData.timestamp = 0;
        errorData.file      = __FILENAME__;
        errorData.line      = __LINE__;

        errorListElement    noError;
        noError.setErrorData(errorData);
        p_searchedError = &noError;
    }
        
    return p_searchedError->getErrorData();      
}    
//setError("TEXT", __FILENAME__, __LINE__);
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
            //Lokalen Error setzen
            this->addErrorToList(p_newError);

            if(this->p_superiorErrorManager != nullptr)
            {
                //Log eintrag wird in superiorErrorManager ausgegeben
                p_superiorErrorManager->setError(ERROR_CODE, FILE, LINE);
            }
            else
            {
                this->log.printErrorSet(ERROR_CODE, FILE, LINE);
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
        //Lokalen Error rücksetzen
        this->deleteErrorFromList(p_errorToDelete);        

        if(this->p_superiorErrorManager != nullptr)
        {
            //Log eintrag wird in superiorErrorManager ausgegeben
            p_superiorErrorManager->resetError(ERROR_CODE, FILE, LINE);
        }
        else
        {
            this->log.printErrorReset(ERROR_CODE, FILE, LINE);
        }
        this->errorCount--;
    }    
}
void BPLC_moduleErrorHandler::resetAllErrors(String FILE, const uint16_t LINE)
{
    this->log.printLog("RESET ALL ERRORS!", FILE, LINE);    
    
    while (this->getErrorCount() > 0)
    {   
        this->resetError(this->p_firstError->getErrorData()->errorCode, FILE, LINE);
    }  
}
void BPLC_moduleErrorHandler::enableErrordetection(String FILE, const uint16_t LINE)
{
    this->enabled = true;
    this->log.printLog("MODULE ERROR DETECTION ENABELD", FILE, LINE);
}
void BPLC_moduleErrorHandler::disableErrordetection(String FILE, const uint16_t LINE)
{
    this->enabled = false;
    this->log.printLog("MODULE ERROR DETECTION DISABLED", FILE, LINE);
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