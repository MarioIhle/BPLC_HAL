#include "BPLC_errorHandler.h"

BPLC_errorHandler::BPLC_errorHandler()
{
    memset(&this->errorCode, 0, sizeof(this->errorCode))
}  
e_BPLC_ERROR_t BPLC_errorHandler::getError()
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
    if(this->error.errorCode == ERROR_CODE)
    {
        memset(this->errorCodes, 0, sizeof(s_errorBufferElement_t));
        this->printLog("reset error code: " + String(ERROR_CODE), FILE, LINE);
    }
}