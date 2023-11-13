#ifndef BPLC_LOG_h
#define BPLC_LOG_h

#include "Arduino.h"

#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"



typedef enum
{
    BPLC_EVENT_LOG__DIN11_CARD__PORT_CHANGED,
    BPLC_EVENT_LOG__REL11_CARD__PORT_WRITE_LOW, 
    BPLC_EVENT_LOG__REL11_CARD__PORT_WRITE_HIGH,

}e_BPLC_EVENT_t;





typedef struct 
{
    e_BPLC_EVENT_t      event;
    uint8_t             port;
    unsigned long       timeStamp;

}s_BASE_LOG_t;





#endif