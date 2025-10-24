#ifndef BPLC_logPrint_h
#define BPLC_logPrint_h

#include "BPLC_errorCodes.h"

#ifdef ARDUINO_AVR_NANO
#define __FILENAME__ (__builtin_strrchr( "/" __FILE__, '/') + 1)
#endif

class BPLC_logPrint
{
    public:
            BPLC_logPrint       ();
    void    printLog            (String TEXT, String FILE, const uint16_t LINE);
    void    printErrorSet       (const e_BPLC_ERROR_t ERROR_CODE,  String FILE, const uint16_t LINE);
    void    printErrorReset     (const e_BPLC_ERROR_t ERROR_CODE,  String FILE, const uint16_t LINE);
    void    printResetReason    (String TEXT, String FILE, const uint16_t LINE);
    void    printRamUsage       ();    
    void    printExtensionCardSimualtionOutput(String EXTENSIONCARD_TYPE, String ADDR, String CHANNEL, String VALUE);
};
#endif 