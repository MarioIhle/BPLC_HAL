#ifndef HAL_interface_h
#define HAL_interface_h

#include "BPLC_ERRORS.h"
#include "BPLC_ioBaseTypes.h"

class halInterface
{
    public:
    virtual void            init                () = 0;
    virtual void            tick                () = 0;
    virtual e_BPLC_ERROR_t  getErrorCode        () = 0;
    virtual void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL) = 0;
};

#endif