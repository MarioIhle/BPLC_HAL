#ifndef BPLC_logPrint_h
#define BPLC_logPrint_h

#include "BPLC_errorCodes.h"

class BPLC_logPrint: ERROR_errorText
{
    public:
            BPLC_logPrint       ();
    void    printLog            (String TEXT, String FILE, const uint16_t LINE);
    void    printError          (const e_BPLC_ERROR_t ERROR_CODE,  String FILE, const uint16_t LINE);
    void    printResetReason    (String TEXT, String FILE, const uint16_t LINE);
};
#endif 