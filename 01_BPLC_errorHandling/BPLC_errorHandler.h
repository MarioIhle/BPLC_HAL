#ifndef BPLC_errorHander_h
#define BPLC_errorHander_h

#include "Arduino.h"
#include "BPLC_errorCodes.h"

#define ERROR_BUFFER_SIZE 10

typedef struct 
{
    e_BPLC_ERROR_t  errorCode;
    uint64_t        timestamp;
    String          file;
    uint16_t        line;

}s_errorBufferElement_t;

class BPLC_errorHandler
{
    public:
                    BPLC_errorHandler       ();
    e_BPLC_ERROR_t  getError                ();
    void            setError                (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);
    void            resetError              (const e_BPLC_ERROR_t ERROR_CODE, String FILE, const uint16_t LINE);

    private:
    s_errorBufferElement_t  error;
};
#endif