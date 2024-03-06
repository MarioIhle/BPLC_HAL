#include "BPLC_ERRORS.h"

BPLC_errorHandler::BPLC_errorHandler()
{
    memset(&this->errorCodes, 0, sizeof(this->errorCodes))
}  
e_BPLC_ERROR_t BPLC_errorHandler::getErrorCode()
{
    return this->error.errorCode;  
}    
void BPLC_errorHandler::setError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    if(this->error.errorCode == BPLC_ERROR__NO_ERROR)
    {
        this->error.timestamp       = millis();
        this->error.file            = FILE;
        this->error.line            = LINE;

        this->printError(ERROR_CODE, FILE, LINE);
    }
}
void BPLC_errorHandler::resetError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    const bool ERROR_IS_SET = (bool)(this->searchStoredErrorCode(ERROR_CODE) != ERROR_BUFFER_SIZE);
    
    if(this->error.errorCode == ERROR_CODE)
    {
        memset(this->errorCodes, 0, sizeof(s_errorBufferElement_t));
        this->printLog("reset error code: " + String(ERROR_CODE), FILE, LINE);
    }
}
void BPLC_errorHandler::resetAllErrors(String FILE, const uint16_t LINE)
{
    memset(&this->errorCodes, 0, sizeof(this->errorCodes))
    this->printLog("reset all errors", FILE, LINE);
}
