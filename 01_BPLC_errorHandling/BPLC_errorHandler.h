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

//Zugriff von übergeordneten Modul
class BPLC_moduleErrorInterface
{
    public:
    virtual e_BPLC_ERROR_t  getModulError  () = 0;
    virtual void            resetModulError(String FILE, const uint16_t LINE) = 0;
};


//Modulinternes Error Handling
class BPLC_errorManager
{
    public:
                    BPLC_errorManager       ();
    
    e_BPLC_ERROR_t  getError                ();
    void            setError                (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void            resetError              (String FILE, const uint16_t LINE);

    //Übergeordneter ErrorManager
    void            setSuperiorErrorManager (BPLC_errorManager*  P_SUPEROIR_ERROR_MANAGER){this->p_superiorErrorManager = P_SUPEROIR_ERROR_MANAGER;}


    private:
    BPLC_errorManager*          p_superiorErrorManager;
    s_errorBufferElement_t      error;
    BPLC_logPrint               log;
};
#endif