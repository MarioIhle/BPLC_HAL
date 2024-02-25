#ifndef hmiEncoder_h
#define hmiEncoder_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "digitalInput.h"

class hmiEncoder:public IO_Interface
{
	public:
                        hmiEncoder              ()
    {
        this->f_invertedDirection   = false;
        this->ioType                = IO_TYPE__ROTARY_ENCODER; 
    }
    void                invertTurningDirection  (){this->f_invertedDirection = true;}
    e_MOVEMENT_t        getTurningDirection     ()
    {        
        e_MOVEMENT_t direction = MOVEMENT__IDLE;    

        if(this->A.islow() && this->B.fallingEdge())
        {
            if(this->f_invertedDirection == false)
            {
                direction = MOVEMENT__RIGHT;
            }
            else
            {
                direction = MOVEMENT__LEFT;
            }        
        }
        else if(this->B.islow() && this->A.fallingEdge())
        {
            if(this->f_invertedDirection == false)
            {
                direction = MOVEMENT__LEFT;
            }
            else
            {
                direction = MOVEMENT__RIGHT;
            }   
        }

        return direction;
    }
    bool                isButtonPressed         (){return this->PB.risingEdge();}

    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return false;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr)
    {
        u_HAL_DATA_t BUFFER;
        BUFFER.digitalIoData.state = P_DATA->encoderData.stateA;
        this->A.halCallback(&BUFFER);
        BUFFER.digitalIoData.state = P_DATA->encoderData.stateB;
        this->B.halCallback(&BUFFER);
        BUFFER.digitalIoData.state = P_DATA->encoderData.stateZ;
        this->PB.halCallback(&BUFFER);
        return BUFFER;
    }
    

    private: 
    e_IO_TYPE_t         ioType;
    bool                f_invertedDirection;
    digitalInput        A;
    digitalInput        B; 
    digitalInput        PB;   	   
};

#endif