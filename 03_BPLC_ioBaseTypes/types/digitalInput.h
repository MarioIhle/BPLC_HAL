#ifndef digitalInput_h
#define digitalInput_h

#include "Arduino.h"
#include "digitalDebouncer.h"
#include "types/00_ioInterface.h"


class digitalInput: public IO_Interface
{
    public:
    //init
                        digitalInput    (){this->ioType = IO_TYPE__DIGITAL_INPUT; this->state = false; this->previousState = false;}
    void                setDebounceTime (const uint8_t LOW_TO_HIGH_MS, const uint8_t HIGH_TO_LOW_MS){this->debouncer.setDebounceTime(LOW_TO_HIGH_MS, HIGH_TO_LOW_MS);}
    //Applikation

    bool 	            ishigh          (){return (this->state == true);}
    bool	            islow           (){return (this->state == false);}
	bool 	            risingEdge      ()
    {
        if(this->f_risingEdgeOccoured)
        {   
            this->f_risingEdgeOccoured  = false;
            return true;
        }        
        return false;
    }
	bool 	            fallingEdge     ()
    {
        if(this->f_fallingEdgeOccoured)
        {   
            this->f_fallingEdgeOccoured  = false;
            return true;
        }        
        return false;
    }
    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    bool                newDataAvailable(){return false;}    
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA)
    {    
        const bool PIN_STATE            = this->state;
        const bool NEW_PIN_STATE        = this->state = this->debouncer.tick(P_DATA->digitalIoData.state);
        const bool PIN_STATE_CHANGED    = (PIN_STATE != NEW_PIN_STATE);
        
        if (PIN_STATE_CHANGED)
        {     
            if(PIN_STATE && !NEW_PIN_STATE)
            {
                this->f_risingEdgeOccoured  = true;
            }
            if(!PIN_STATE && NEW_PIN_STATE)
            {
                this->f_fallingEdgeOccoured = true;
            }
            this->state = NEW_PIN_STATE;
        }
        return *P_DATA;
    }

    private:
    bool                f_fallingEdgeOccoured;
    bool                f_risingEdgeOccoured;

    bool                state;                      
    bool                previousState;
    e_IO_TYPE_t         ioType;
    digitalDebouncer    debouncer;
};

#endif