#include "BPLC_errorHandler.h"

BPLC_errorHandler::BPLC_errorHandler()
{
    this->error.errorCode = BPLC_ERROR__NO_ERROR;
    this->error.timestamp       = 0;
    this->error.file            = "";
    this->error.line            = 0;
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

        //this->printError(ERROR_CODE, FILE, LINE);
    }
}
void BPLC_errorHandler::resetError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    if(this->error.errorCode == ERROR_CODE)
    {
        this->error.errorCode = BPLC_ERROR__NO_ERROR;
        this->error.timestamp       = 0;
        this->error.file            = "";
        this->error.line            = 0;
        //this->printLog("reset error code: " + String(ERROR_CODE), FILE, LINE);
    }
}