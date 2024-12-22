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
        this->A.setDebounceTime(50,50);
        this->B.setDebounceTime(50,50);
        this->PB.setDebounceTime(50,50);
    }

    void                invertTurningDirection  (){this->f_invertedDirection = !this->f_invertedDirection;}
    e_MOVEMENT_t        getTurningDirection     (){e_MOVEMENT_t DIRECTION = this->direction; this->direction = MOVEMENT__IDLE; return DIRECTION;}
    bool                buttonPressed           (){return this->PB.fallingEdge();}
    bool                buttonReleased          (){return this->PB.risingEdge();}
    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return false;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr)
    {              
        u_HAL_DATA_t EXTRACTED_HAL_DATA__A;
        EXTRACTED_HAL_DATA__A.digitalIoData.state = P_DATA->encoderData.stateA;
        u_HAL_DATA_t EXTRACTED_HAL_DATA__B ;
        EXTRACTED_HAL_DATA__B.digitalIoData.state = P_DATA->encoderData.stateB;
        u_HAL_DATA_t EXTRACTED_HAL_DATA__Z;
        EXTRACTED_HAL_DATA__Z.digitalIoData.state = P_DATA->encoderData.stateZ;

        this->A.halCallback(&EXTRACTED_HAL_DATA__A);   
        this->B.halCallback(&EXTRACTED_HAL_DATA__B);   
        this->PB.halCallback(&EXTRACTED_HAL_DATA__Z);  

        //Richtungsauswertung
        if(this->A.risingEdge() && this->B.ishigh())                   
        {     
            if(this->f_invertedDirection)
            {
                this->direction = MOVEMENT__LEFT;
            }
            else
            {
                this->direction = MOVEMENT__RIGHT;
            }    
        }
        else if (this->A.ishigh() && this->B.risingEdge())
        {
            if(this->f_invertedDirection)
            {
                this->direction = MOVEMENT__RIGHT;
            }
            else
            {
                this->direction = MOVEMENT__LEFT;
            }            
        }                                
        else
        {
            this->direction = MOVEMENT__IDLE;
        }
        return *P_DATA;
    }
    

    private: 

    e_IO_TYPE_t         ioType;
    bool                f_invertedDirection;
    digitalInput        A;
    digitalInput        B;
    digitalInput        PB;
    e_MOVEMENT_t        direction;
};

#endif