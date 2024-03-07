#include "BPLC_errorHandler.h"

BPLC_moduleErrorHandler::BPLC_moduleErrorHandler()
{
    this->error.errorCode = BPLC_ERROR__NO_ERROR;
    this->error.timestamp       = 0;
    this->error.file            = "";
    this->error.line            = 0;
}  
e_BPLC_ERROR_t BPLC_moduleErrorHandler::getError()
{
    return this->error.errorCode;  
}    
void BPLC_moduleErrorHandler::setError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    if(this->error.errorCode == BPLC_ERROR__NO_ERROR)
    {
        this->error.errorCode       = ERROR_CODE;
        this->error.timestamp       = millis();
        this->error.file            = FILE;
        this->error.line            = LINE;

        this->log.printError(ERROR_CODE, FILE, LINE);
    }
}
void BPLC_moduleErrorHandler::resetError(String FILE, const uint16_t LINE)
{
    this->error.errorCode       = BPLC_ERROR__NO_ERROR;
    this->error.timestamp       = 0;
    this->error.file            = "";
    this->error.line            = 0;

    this->log.printLog("Reset module error!", FILE, LINE);
}