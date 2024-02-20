#ifndef digitalInput_h
#define digitalInput_h

#include "Arduino.h"
#include "ioInterface.h"

class digitalInput: public IO_Interface
{
    public:
    //init
                        digitalInput    (){this->ioType = IO_TYPE__DIGITAL_INPUT; this->state = false; this->previousState = false;}
    //Applikation

    bool 	            ishigh          (){return (bool)(this->state == true && this->previousState == true);}
    bool	            islow           (){return (bool)(this->state == false && this->previousState == false);}
	bool 	            risingEdge      ();	
	bool 	            fallingEdge     (){return (bool)(this->state == false && this->previousState == true);}	
    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    bool                newDataAvailable(){return false;}
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA);


    private:
    bool                f_fallingEdgeOccoured;
    bool                f_risingEdgeOccoured;

    bool                state;                      
    bool                previousState;
    e_IO_TYPE_t         ioType;
};

bool digitalInput::risingEdge()
{
    return this->f_risingEdgeOccoured;
}
u_HAL_DATA_t digitalInput::halCallback(u_HAL_DATA_t* P_DATA)
{
    this->previousState = this->state; 
    this->state = P_DATA->digitalIoData.state; 

    if(this->state == false && this->previousState == true)
    {
        this->f_fallingEdgeOccoured = true;
    }
    if(this->state == true && this->previousState == false)
    {
        this->f_risingEdgeOccoured = true;
    }
    return *P_DATA;
}

#endif