#ifndef BPLC_LOG_h
#define BPLC_LOG_h

#include "Arduino.h"

#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"


typedef enum
{
    BPLC_EVENT_LOG__NO_EVENT_IN_THIS_SLOT,
    BPLC_EVENT_LOG__DIN11_CARD_PORT_CHANGED,
    BPLC_EVENT_LOG__REL11_CARD_PORT_WRITE_LOW, 
    BPLC_EVENT_LOG__REL11_CARD_PORT_WRITE_HIGH,
    BPLC_EVENT_LOG__MAP_OBJECT_TO_REL11_CARD,

}e_BPLC_EVENT_t;


typedef struct data
{
    e_BPLC_EVENT_t      event;
    uint8_t             payload;
    unsigned long       timeStamp;

}s_BASE_LOG_t;

typedef struct 
{
    s_BASE_LOG_t    loggedEvent;
    bool            f_printedOnOled;
    bool            f_printedOnSerialMonitor;
}s_eventBuffer_t;


#define MAX_EVENTS_PER_CYCLE 50

class BPLC_log
{
    public:
    BPLC_log();

    void tick();
    void logEvent(const e_BPLC_EVENT_t EVENT_TYPE, const uint8_t PAYLOAD, unsigned long TIMESTAMP);
    void logEvent(const e_BPLC_EVENT_t EVENT_TYPE, const float   PAYLOAD, unsigned long TIMESTAMP);
    void logEvent(const e_BPLC_EVENT_t EVENT_TYPE, const int16_t PAYLOAD, unsigned long TIMESTAMP);


    private:
    
    void            printObjectMappedToREL11Card(const s_BASE_LOG_t* P_LOG);
    
    s_eventBuffer_t eventBuffer[MAX_EVENTS_PER_CYCLE];
};
#endif