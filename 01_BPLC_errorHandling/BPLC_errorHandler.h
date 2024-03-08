#ifndef BPLC_errorHander_h
#define BPLC_errorHander_h

#include "Arduino.h"
#include "BPLC_errorCodes.h"
#include "BPLC_logPrint.h"

#define ERROR_BUFFER_SIZE 10

typedef struct 
{
    e_BPLC_ERROR_t  errorCode;
    uint64_t        timestamp;
    String          file;
    uint16_t        line;

}s_errorBufferElement_t;

class BPLC_ERROR
{
    public:
                            BPLC_ERROR      (){};
    void                    setNext         (BPLC_ERROR* P_NEXT){this->p_nextError = P_NEXT;}
    BPLC_ERROR*             getNext         (){return this->p_nextError;}
    void                    setErrorData    (s_errorBufferElement_t ERROR_DATA){this->errorData = ERROR_DATA;}
    s_errorBufferElement_t  getErrorData    (){return this->errorData;}

    private:
    BPLC_ERROR*     p_nextError;

    s_errorBufferElement_t errorData;
};

//Zugriff von übergeordneten Modul
class BPLC_moduleErrorInterface
{
    public:
    virtual uint8_t         getModulErrorCount  () = 0;
    virtual e_BPLC_ERROR_t  getModulError       () = 0;
    virtual void            resetModulError     (String FILE, const uint16_t LINE) = 0;
};

//Modulinternes Error Handling
class BPLC_moduleErrorHandler
{
    public:
                            BPLC_moduleErrorHandler ();
    uint8_t                 getErrorCount           ();
    s_errorBufferElement_t* getError                (uint8_t ERROR_NUMBER = 0);
    
    void                    setError                (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void                    resetError              (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);

    private:
    BPLC_ERROR*             searchError             (const e_BPLC_ERROR_t ERROR_CODE);
    void                    addErrorToList          (BPLC_ERROR* ERROR_TO_ADD);
    void                    deleteErrorFromList     (BPLC_ERROR* ERROR_TO_DELETE);

    BPLC_ERROR*             p_firstError;
    BPLC_logPrint           log;
};
#endif