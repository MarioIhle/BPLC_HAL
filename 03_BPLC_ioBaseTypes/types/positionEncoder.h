#ifndef positionEncoder_h
#define positionEncoder_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "digitalInput.h"

class positionEncoder:public IO_Interface
{
	public:
                        positionEncoder                 (const uint16_t PULSES_PER_REV){this->pulsesPerRevolution = PULSES_PER_REV; this->ioType = IO_TYPE__POSITION_ENCODER; this->f_zIndexIsKnown = false;}
    uint16_t            getPositionInDegree             ()
    {
        if(this->f_zIndexIsKnown)
        {
            return map(this->poitionIncrement.getCount(), 0 , this->pulsesPerRevolution, 0, 360);
        }
        else
        {
            return false;
        }
    }
    uint16_t            getPositionIncrement            ()
    {
        if(this->f_zIndexIsKnown)
        {
            return this->poitionIncrement.getCount();
        }  
        else
        {
            return false;
        }
    }

    //Hal handling
    e_IO_TYPE_t         getIoType                       (){return this->ioType;}
    bool                newDataAvailable                (){return false;}
    u_HAL_DATA_t        halCallback                     (u_HAL_DATA_t* P_DATA = nullptr)
    {
        u_HAL_DATA_t BUFFER;
        BUFFER.digitalIoData.state = P_DATA->encoderData.stateA;
        this->A.halCallback(&BUFFER);
        BUFFER.digitalIoData.state = P_DATA->encoderData.stateB;
        this->B.halCallback(&BUFFER);
        BUFFER.digitalIoData.state = P_DATA->encoderData.stateZ;
        this->Z.halCallback(&BUFFER);
        //"links"                                                
        if((this->A.fallingEdge()   && this->B.ishigh())     
        || (this->A.islow()         && this->B.fallingEdge())    
        || (this->A.risingEdge()    && this->B.islow())    
        || (this->A.ishigh()        && this->B.risingEdge()))   
        {
            this->poitionIncrement.increment();
        }
        //"rechts"                                            
        if((this->A.ishigh()        && this->B.fallingEdge())    
        || (this->A.fallingEdge()   && this->B.islow())    
        || (this->A.islow()         && this->B.risingEdge())    
        || (this->A.risingEdge()    && this->B.ishigh()))   
        {
            this->poitionIncrement.decrement();
        }
        //Z-index
        if(this->Z.risingEdge())
        {
            this->poitionIncrement.resetCount();
        }
        return BUFFER;
    }
    

    private: 
    e_IO_TYPE_t         ioType;
    counter             poitionIncrement;
    uint16_t            pulsesPerRevolution;
    digitalInput        A;
    digitalInput        B; 
    digitalInput        Z;   	   
    bool                f_zIndexIsKnown;
};

#endif