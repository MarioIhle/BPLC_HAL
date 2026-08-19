#ifndef BPLC_errorHander_h
#define BPLC_errorHander_h

#include "Arduino.h"
#include "BPLC_errorCodes.h"
#include "BPLC_logPrint.h"

//Verhindern dass Ram überläuft 
#define ERROR_BUFFER_SIZE 10

class BPLC_moduleErrorHandler;

typedef struct 
{
    e_BPLC_ERROR_t  errorCode;
    uint64_t        timestamp;
    String          file;
    uint16_t        line;
    BPLC_moduleErrorHandler* p_source;

}s_error_t;

//Modulinternes Error Handling
class BPLC_moduleErrorHandler
{
    public:
                            BPLC_moduleErrorHandler ();
                            ~BPLC_moduleErrorHandler();
                            BPLC_moduleErrorHandler(const BPLC_moduleErrorHandler&) = delete;
    BPLC_moduleErrorHandler& operator=(const BPLC_moduleErrorHandler&) = delete;
    bool                    noErrorSet              ();
    uint8_t                 getErrorCount           ();
    s_error_t*              getError                (uint8_t ERROR_NUMBER = 0);
    e_BPLC_ERROR_t          getErrorCode            (){return this->getError()->errorCode;}
    
    void                    setError                (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void                    setErrorFromModule      (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE, BPLC_moduleErrorHandler* P_SOURCE);
    void                    resetError              (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void                    resetErrorFromModule    (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE, BPLC_moduleErrorHandler* P_SOURCE);
    void                    resetAllErrors          (String FILE, const uint16_t LINE);
    //enable/disbale
    void enableErrordetection       (String FILE, const uint16_t LINE);
    void disableErrordetection      (String FILE, const uint16_t LINE);
    bool errorDetectionisEnabled    (){return this->enabled;}                   
    //Übergeordneter Errorhandler 
    void                        setSuperiorErrorHandler (BPLC_moduleErrorHandler* p_errorHandler);
    BPLC_moduleErrorHandler*    getSuperiorErrorHandler ();

    private:
    bool                    enabled;                 
    BPLC_logPrint           log;
    //Error Listen handling
    int8_t                  searchError             (const e_BPLC_ERROR_t ERROR_CODE, BPLC_moduleErrorHandler* P_SOURCE);
    s_error_t               errorData[ERROR_BUFFER_SIZE];
    bool                    f_errorActive[ERROR_BUFFER_SIZE];
    uint8_t                 errorCount;  
    s_error_t               noErrorData;

    BPLC_moduleErrorHandler* p_superiorErrorHandler;
};


//Zugriff von übergeordneten Modul auf ModulErrorHandler
class BPLC_moduleErrorInterface
{
    public:
    virtual uint8_t         getModuleErrorCount                 () = 0;
    virtual e_BPLC_ERROR_t  getModuleErrorCode                  (uint8_t ERROR_NUMBER) = 0;    
    virtual void            resetAllModuleErrors                (String FILE, const uint16_t LINE) = 0;
    virtual void            setSuperiorErrorHandlerForModule    (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER) = 0;
};
#endif