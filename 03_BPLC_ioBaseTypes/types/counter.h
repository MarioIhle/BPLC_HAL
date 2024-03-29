#ifndef counter_h
#define counter_h

#include "Arduino.h"
#include "00_ioInterface.h"


class counter: public IO_Interface
{
    public:
    //init
                        counter         (){this->ioType = IO_TYPE__DIGITAL_COUNTER; this->count = 0;}
    //Applikation
    uint64_t 	        getCount        (){return this->count;}
    void                setCount        (uint64_t COUNT){this->count = COUNT;}
    void	            resetCount      (){this->count = 0;}
	void 	            increment       (){this->count++;}	
    void 	            decrement       ()
    {
        this->count--; 
        if( this->count < 0)
        {
            this->count = 0;
        }
    }
    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    bool                newDataAvailable(){return false;}
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA){if(P_DATA->digitalIoData.state == true && this->previousState == false){this->count++;} this->previousState = P_DATA->digitalIoData.state; return *P_DATA;}
    

    private:
    volatile uint64_t   count; 
    e_IO_TYPE_t         ioType;
    volatile bool       previousState;
};

#endif