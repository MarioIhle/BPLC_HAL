#include "BPLC_errorHandler.h"
//Public
BPLC_moduleErrorHandler::BPLC_moduleErrorHandler()
{
    this->p_superiorErrorHandler    = nullptr;
    this->errorCount                = 0;
    this->enabled                   = true;
    for(uint8_t errorNumber = 0; errorNumber < ERROR_BUFFER_SIZE; errorNumber++)
    {
        this->f_errorActive[errorNumber] = false;
    }
    this->noErrorData.errorCode     = BPLC_ERROR__NO_ERROR;
    this->noErrorData.timestamp     = 0;
    this->noErrorData.file          = __FILENAME__;
    this->noErrorData.line          = 0;
    this->noErrorData.p_source      = nullptr;
}
BPLC_moduleErrorHandler::~BPLC_moduleErrorHandler()
{}
bool BPLC_moduleErrorHandler::noErrorSet()
{    
    const bool  NO_ERROR_SET                     = (this->errorCount == 0);
    const bool  THIS_ERROR_HANDLER_DISABLED      = (!this->enabled);
    
    return (NO_ERROR_SET || THIS_ERROR_HANDLER_DISABLED);
}
uint8_t BPLC_moduleErrorHandler::getErrorCount()
{
    return this->errorCount;
}  
s_error_t* BPLC_moduleErrorHandler::getError(uint8_t ERROR_NUMBER)
{
    uint8_t activeErrorNumber = 0;
    for(uint8_t errorSlot = 0; errorSlot < ERROR_BUFFER_SIZE; errorSlot++)
    {
        if(this->f_errorActive[errorSlot])
        {
            if(activeErrorNumber == ERROR_NUMBER)
            {
                return &this->errorData[errorSlot];
            }
            activeErrorNumber++;
        }
    }
    return &this->noErrorData;
}    
//setError("TEXT", __FILENAME__, __LINE__);
void BPLC_moduleErrorHandler::setError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    this->setErrorFromModule(ERROR_CODE, FILE, LINE, this);
}
void BPLC_moduleErrorHandler::setErrorFromModule(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE, BPLC_moduleErrorHandler* P_SOURCE)
{
    if(this->enabled)
    {
        //Nur speichern, wenn noch nicht vorhanden
        const bool ERROR_NOT_SET_YET = (this->searchError(ERROR_CODE, P_SOURCE) < 0);
       
        if(ERROR_NOT_SET_YET)
        {
            s_error_t ERROR_DATA;
            ERROR_DATA.errorCode    = ERROR_CODE;
            ERROR_DATA.timestamp    = millis();
            ERROR_DATA.file         = FILE;
            ERROR_DATA.line         = LINE;
            ERROR_DATA.p_source     = P_SOURCE;

            //Error buffer voll, keine Error objekte mehr erzeugem, da sonst Ram überläuft
            if(this->errorCount < ERROR_BUFFER_SIZE)
            {
                uint8_t errorSlot = 0;
                while(this->f_errorActive[errorSlot])
                {
                    errorSlot++;
                }
                for(uint8_t slot = errorSlot; slot > 0; slot--)
                {
                    this->errorData[slot] = this->errorData[slot - 1];
                    this->f_errorActive[slot] = this->f_errorActive[slot - 1];
                }
                this->errorData[0] = ERROR_DATA;
                this->f_errorActive[0] = true;

                if(this->p_superiorErrorHandler != nullptr)
                {
                    //Log eintrag wird in superiorErrorManager ausgegeben
                    p_superiorErrorHandler->setErrorFromModule(ERROR_CODE, FILE, LINE, P_SOURCE);
                }
                else
                {
                    this->log.printErrorSet(ERROR_CODE, FILE, LINE);
                }                
                this->errorCount++;
            }
        }
    }
}
void BPLC_moduleErrorHandler::resetError(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE)
{
    this->resetErrorFromModule(ERROR_CODE, FILE, LINE, this);
}
void BPLC_moduleErrorHandler::resetErrorFromModule(const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE, BPLC_moduleErrorHandler* P_SOURCE)
{
    const int8_t ERROR_SLOT = this->searchError(ERROR_CODE, P_SOURCE);

    if(ERROR_SLOT >= 0)
    {
        for(uint8_t slot = (uint8_t)ERROR_SLOT; slot + 1 < ERROR_BUFFER_SIZE; slot++)
        {
            this->errorData[slot] = this->errorData[slot + 1];
            this->f_errorActive[slot] = this->f_errorActive[slot + 1];
        }
        this->f_errorActive[ERROR_BUFFER_SIZE - 1] = false;

        if(this->p_superiorErrorHandler != nullptr)
        {
            //Log eintrag wird in superiorErrorManager ausgegeben
            p_superiorErrorHandler->resetErrorFromModule(ERROR_CODE, FILE, LINE, P_SOURCE);
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
    
    while(this->errorCount > 0)
    {   
        const e_BPLC_ERROR_t ERROR_CODE = this->getError()->errorCode;
        BPLC_moduleErrorHandler* p_source = this->getError()->p_source;
        this->resetErrorFromModule(ERROR_CODE, FILE, LINE, p_source);
    }  
}
void BPLC_moduleErrorHandler::enableErrordetection(String FILE, const uint16_t LINE)
{
    this->enabled = true;
    this->log.printLog("MODULE ERROR DETECTION ENABELD", FILE, LINE);
}
void BPLC_moduleErrorHandler::disableErrordetection(String FILE, const uint16_t LINE)
{    
    this->log.printLog("MODULE ERROR DETECTION DISABLED", FILE, LINE);
    this->enabled = false;
    while(this->errorCount > 0)
    {
        const e_BPLC_ERROR_t ERROR_CODE = this->getError()->errorCode;
        BPLC_moduleErrorHandler* p_source = this->getError()->p_source;
        this->resetErrorFromModule(ERROR_CODE, FILE, LINE, p_source);
    }
}
//Übergeordneter Errorhandler 
void BPLC_moduleErrorHandler::setSuperiorErrorHandler (BPLC_moduleErrorHandler* p_errorHandler)
{
        if(p_errorHandler != this)
        {
                this->p_superiorErrorHandler = p_errorHandler;
        }
}
BPLC_moduleErrorHandler*  BPLC_moduleErrorHandler::getSuperiorErrorHandler ()
{
    return this->p_superiorErrorHandler;
}
//Private Listenhandling
int8_t BPLC_moduleErrorHandler::searchError(const e_BPLC_ERROR_t ERROR_CODE, BPLC_moduleErrorHandler* P_SOURCE)
{
    for(uint8_t errorSlot = 0; errorSlot < ERROR_BUFFER_SIZE; errorSlot++)
    {        
        if(this->f_errorActive[errorSlot]
        && (this->errorData[errorSlot].errorCode == ERROR_CODE)
        && (this->errorData[errorSlot].p_source == P_SOURCE))
        {
            return (int8_t)errorSlot;
        }
    }
    return -1;
}