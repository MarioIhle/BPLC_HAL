#ifndef BPLC_errorHander_h
#define BPLC_errorHander_h

#include "Arduino.h"
#include "BPLC_errorCodes.h"
#include "BPLC_logPrint.h"

//Verhindern dass Ram überläuft 
#define ERROR_BUFFER_SIZE 10

typedef struct 
{
    e_BPLC_ERROR_t  errorCode;
    uint64_t        timestamp;
    String          file;
    uint16_t        line;

}s_error_t;

class errorListElement
{
    public:
                            errorListElement    (){p_nextError = nullptr; memset(&this->errorData, 0, sizeof(this->errorData));}
    void                    setNextError        (errorListElement* P_NEXT){this->p_nextError = P_NEXT;}
    errorListElement*       getNextError        (){return this->p_nextError;}
    void                    setErrorData        (s_error_t ERROR_DATA){this->errorData = ERROR_DATA;}
    s_error_t*              getErrorData        (){return &this->errorData;}

    private:
    errorListElement*       p_nextError;

    s_error_t errorData;
};


//Modulinternes Error Handling
class BPLC_moduleErrorHandler
{
    public:
                            BPLC_moduleErrorHandler ();
    bool                    noErrorSet              (){return (this->errorCount == 0);}
    uint8_t                 getErrorCount           ();
    s_error_t*              getError                (uint8_t ERROR_NUMBER = 0);
    e_BPLC_ERROR_t          getErrorCode            (){return this->p_firstError->getErrorData()->errorCode;}
    
    void                    setError                (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void                    resetError              (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void                    resetAllErrors          (String FILE, const uint16_t LINE);
    void                    enableErrordetection    (String FILE, const uint16_t LINE);
    void                    disableErrordetection   (String FILE, const uint16_t LINE);


    private:
    bool                    enabled;                 
    BPLC_logPrint           log;
    //Error Listen handling
    errorListElement*       searchError             (const e_BPLC_ERROR_t ERROR_CODE);
    void                    addErrorToList          (errorListElement* ERROR_TO_ADD);
    void                    deleteErrorFromList     (errorListElement* ERROR_TO_DELETE);
    errorListElement*       p_firstError;  
    uint8_t                 errorCount;  

    protected:
    //Optional error an übergeordneter ErrorHandler weiter geben
    BPLC_moduleErrorHandler* p_superiorErrorManager;
};


//Zugriff von übergeordneten Modul auf ModulErrorManager
class BPLC_moduleErrorInterface
{
    public:
    virtual uint8_t         getModuleErrorCount     () = 0;
    virtual e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER) = 0;    
    virtual void            resetAllModuleErrors    (String FILE, const uint16_t LINE) = 0;
    virtual void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER) = 0;
};
#endif