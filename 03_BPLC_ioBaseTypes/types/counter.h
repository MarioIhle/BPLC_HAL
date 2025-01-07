#ifndef counter_h
#define counter_h

#include "Arduino.h"
#include "00_ioInterface.h"


class counter: public IO_Interface
{
    public:
    //init
                        counter         (){this->ioType = IO_TYPE__DIGITAL_COUNTER; this->count = 0; this->f_countingEnabled = true;}
    //Applikation
    void                disableCounter  (){this->f_countingEnabled = false;}
    void                enableCounter   (){this->f_countingEnabled = true;}
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
    bool                newDataAvailable(){this->count++;return false;} //cout++ ist ein kleiner Hack um über die INT isr direkt zählen zu können 
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA)
    {
        if((P_DATA->digitalIoData.state == true) && (this->previousState == false)
        && this->f_countingEnabled)
        {
            this->count++;
        } 
        this->previousState = P_DATA->digitalIoData.state; 
        return *P_DATA;
    }


    private:
    volatile uint64_t   count; 
    e_IO_TYPE_t         ioType;
    volatile bool       previousState;
    bool                f_countingEnabled;  //Zählen für das auswerten der Drehzahl kurz pausieren
};
#endif