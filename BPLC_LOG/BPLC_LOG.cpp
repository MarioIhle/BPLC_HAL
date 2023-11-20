#include "BPLC_LOG.h"

BPLC_log::BPLC_log()
{}

void BPLC_log::tick()
{
     for(uint8_t BUFFER_SLOT = 0; BUFFER_SLOT < MAX_EVENTS_PER_CYCLE; BUFFER_SLOT++)
    {
        const bool BUFFER_SLOT_NOT_EMPTY                = (bool)(this->eventBuffer[BUFFER_SLOT].loggedEvent.event != BPLC_EVENT_LOG__NO_EVENT_IN_THIS_SLOT);
        const bool NOT_PRINTETED_ON_SERIAL_MONITOT_YET  = (bool)(this->eventBuffer[BUFFER_SLOT].f_printedOnSerialMonitor == false);

        if(BUFFER_SLOT_NOT_EMPTY && NOT_PRINTETED_ON_SERIAL_MONITOT_YET)
        {
            switch (this->eventBuffer[BUFFER_SLOT].loggedEvent.event)
            {
            case BPLC_EVENT_LOG__MAP_OBJECT_TO_REL11_CARD:
                this->printObjectMappedToREL11Card(&this->eventBuffer[BUFFER_SLOT].loggedEvent);
                break;
            
            default:
                break;
            }

            this->eventBuffer[BUFFER_SLOT].f_printedOnSerialMonitor = true;
        }
    }
}

void BPLC_log::logEvent(const e_BPLC_EVENT_t EVENT_TYPE, const uint8_t PAYLOAD, unsigned long TIMESTAMP)
{
    for(uint8_t BUFFER_SLOT = 0; BUFFER_SLOT < MAX_EVENTS_PER_CYCLE; BUFFER_SLOT++)
    {
        if(this->eventBuffer[BUFFER_SLOT].loggedEvent.event == BPLC_EVENT_LOG__NO_EVENT_IN_THIS_SLOT)
        {
            this->eventBuffer[BUFFER_SLOT].loggedEvent.event        = EVENT_TYPE;
            this->eventBuffer[BUFFER_SLOT].loggedEvent.payload      = PAYLOAD;
            this->eventBuffer[BUFFER_SLOT].loggedEvent.timeStamp    = TIMESTAMP;
        }
    }   
}

void BPLC_log::logEvent(const e_BPLC_EVENT_t EVENT_TYPE, const float   PAYLOAD, unsigned long TIMESTAMP)
{}

void BPLC_log::logEvent(const e_BPLC_EVENT_t EVENT_TYPE, const int16_t PAYLOAD, unsigned long TIMESTAMP)
{}

void BPLC_log::printObjectMappedToREL11Card(const s_BASE_LOG_t* P_LOG)
{
    Serial.println("##############################");  
    Serial.print("map object to REL11_CARD "); Serial.println(P_LOG->payload);
    Serial.print("TimeStamp: "); Serial.println(P_LOG->timeStamp);
}